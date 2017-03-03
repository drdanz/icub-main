// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-


/* Copyright (C) 2012  iCub Facility, Istituto Italiano di Tecnologia
 * Author: Valentina Gaggero
 * email: valentina.gaggero@iit.it
 * Permission is granted to copy, distribute, and/or modify this program
 * under the terms of the GNU General Public License, version 2 or any
 * later version published by the Free Software Foundation.
 *
 * A copy of the license can be found at
 * http://www.robotcub.org/icub/license/gpl.txt
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details
 */

// update comment hereafter

/**
 * @ingroup icub_hardware_modules
 * \defgroup eth2ems eth2ems
 *
 * Implements <a href="http://wiki.icub.org/yarpdoc/d3/d5b/classyarp_1_1dev_1_1ICanBus.html" ICanBus interface <\a> for a ems to can bus device.
 * This is the eth2ems module device.
 *
 * Copyright (C) 2012 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 *
 * Author: Valentina Gaggero
 *
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 * This file can be edited at src/modules/....h
 *
 */



#ifndef __mcParserh__
#define __mcParserh__



#include <string>
#include <vector>
#include <map>

//  Yarp stuff
#include <yarp/os/Bottle.h>
//#include <yarp/os/Time.h>
#include <yarp/os/Bottle.h>
#include <yarp/dev/ControlBoardPid.h>
#include <yarp/dev/ControlBoardHelper.h>
#include <yarp/os/ConstString.h>

#include <yarp/dev/ControlBoardInterfacesImpl.h>
#include <yarp/dev/ControlBoardInterfacesImpl.inl>

#include "EoMotionControl.h"
#include <yarp/os/LogStream.h>







// - public #define  --------------------------------------------------------------------------------------------------



typedef enum
{
    PidAlgo_simple = 0,
    PIdAlgo_velocityInnerLoop = 1,
    PidAlgo_currentInnerLoop =2
} PidAlgorithmType_t;


typedef enum
{
    controlUnits_machine = 0,
    controlUnits_metric = 1,
    controlUnits_unknown = 255
} GenericControlUnitsType_t;

class Pid_Algorithm
{
public:
    PidAlgorithmType_t type;
    GenericControlUnitsType_t ctrlUnitsType;
    virtual ~Pid_Algorithm() {;};

};


class Pid_Algorithm_simple: public Pid_Algorithm
{
public:
    yarp::dev::Pid *pid;
    Pid_Algorithm_simple(int nj)
    {
        pid = new yarp::dev::Pid[nj];
    };
    ~Pid_Algorithm_simple()
    {
        if(pid)
        {
            delete[]pid;
        }
    };

};

class PidAlgorithm_VelocityInnerLoop: public Pid_Algorithm
{
public:
    yarp::dev::Pid *extPid; //pos, trq, velocity
    yarp::dev::Pid *innerVelPid;
    PidAlgorithm_VelocityInnerLoop(int nj)
    {
        extPid = new yarp::dev::Pid[nj];
        innerVelPid = new yarp::dev::Pid[nj];
    };
    ~PidAlgorithm_VelocityInnerLoop()
    {
        if(extPid)
        {
            delete[]extPid;
        }
         if(innerVelPid)
        {
            delete[]innerVelPid;
        }
    }
};


// class PidAlgorithm_CurrentInnerLoop: public Pid_Algorithm
// {
// public:
//     yarp::dev::Pid *extPid;
//     yarp::dev::Pid *innerCurrLoop;
//     PidAlgorithm_CurrentInnerLoop(int nj)
//     {
//         extPid = allocAndCheck<yarp::dev::Pid>(nj);
//         innerCurrLoop = allocAndCheck<yarp::dev::Pid>(nj);
//     };
//     ~PidAlgorithm_CurrentInnerLoop()
//     {
//         checkAndDestroy(extPid);
//         checkAndDestroy(innerCurrLoop);
//     }
// };

class eomcParser_pidInfo
{
public:

    yarp::dev::Pid pid;
    GenericControlUnitsType_t ctrlUnitsType;
    PidAlgorithmType_t controlLaw;
    std::string usernamePidSelected;
    bool enabled;

    eomcParser_pidInfo()//:usernamePidSelected("none")
    {
        enabled = false;
        controlLaw = PidAlgo_simple;
        ctrlUnitsType = controlUnits_machine;
    }
    ~eomcParser_pidInfo()
    {
        //delete (usernamePidSelected);
    }

    void dumpdata(void);

};

class eomcParser_trqPidInfo : public eomcParser_pidInfo
{
public:
    double kbemf;                             /** back-emf compensation parameter */
    double ktau;                              /** motor torque constant */
    int    filterType;
};



typedef struct
{
    bool hasHallSensor;
    bool hasTempSensor;
    bool hasRotorEncoder;
    bool hasRotorEncoderIndex;
    int  rotorIndexOffset;
    int  motorPoles;
    bool hasSpeedEncoder ; //facoltativo
} eomc_twofocSpecificInfo;


class eomc_jointsSet
{
public:
    int           id; //num of set. it can be between 0 and max number of joint (_njoints)
    std::vector<int>   joints; //list of joints belongig to this set2joint

    eOmc_jointset_configuration_t cfg;

    eomc_jointsSet(int num=0)
    {
        id=num;
        joints.resize(0);
        cfg.candotorquecontrol=0;
        cfg.usespeedfeedbackfrommotors=0;
        cfg.pidoutputtype=eomc_pidoutputtype_unknown;
        cfg.dummy=0;
        cfg.constraints.type=eomc_jsetconstraint_unknown;
        cfg.constraints.param1=0;
        cfg.constraints.param2=0;
    }
public:
    int getNumberofJoints(void) {return (joints.size());}
    eOmc_jointset_configuration_t* getConfiguration(void) {return &cfg;}
    void setUseSpeedFeedbackFromMotors(bool flag){cfg.usespeedfeedbackfrommotors = flag;}
    void setPidOutputType(eOmc_pidoutputtype_t type){cfg.pidoutputtype = type;}
    void setCanDoTorqueControl(bool flag) {cfg.candotorquecontrol = flag;}
    void dumpdata();
};

typedef struct
{
    int velocity;
}eomc_timeouts_t;

typedef struct
{
    double nominalCurrent;
    double peakCurrent;
    double overloadCurrent;
}eomc_motorCurrentLimits;


typedef struct
{
    double posMin;                         /** user joint limits, max*/
    double posMax;                         /** user joint limits, min*/
    double posHwMax;                       /** hardaware joint limits, max */
    double posHwMin;                       /** hardaware joint limits, min */
    double velMax;
}eomc_jointLimits;

typedef struct
{
    double posMin;
    double posMax;
    double pwmMax;
}eomc_rotorLimits;


typedef struct
{
    std::vector<double>                  matrixJ2M;
    std::vector<double>                  matrixM2J;
    std::vector<double>                  matrixE2J;
} eomc_couplingInfo_t;

typedef struct
{
    int             mappedto;
    std::string     name;
    JointTypeEnum   type;
}eomc_axisInfo_t;

typedef struct
{
    double min_stiff;
    double max_stiff;
    double min_damp;
    double max_damp;
    double param_a;
    double param_b;
    double param_c;
} eomc_impedanceLimits;


typedef struct
{
    double stiffness;
    double damping;
    eomc_impedanceLimits limits;
}eomc_impedanceParameters;

//template <class T>

class mcParser
{

private:
    int _njoints;
    std::string _boardname;
    bool _verbosewhenok;

    std::map<std::string, Pid_Algorithm*> posAlgoMap;
    std::map<std::string, Pid_Algorithm*> velAlgoMap;
    std::map<std::string, Pid_Algorithm*> trqAlgoMap;

    std::vector<std::string> _posistionControlLaw;
    std::vector<std::string> _velocityControlLaw;
    std::vector<std::string> _torqueControlLaw;
    std::vector<std::string> _currentControlLaw;

    double *_kbemf;                             /** back-emf compensation parameter */
    double *_ktau;                              /** motor torque constant */
    int * _filterType;





    //PID parsing functions
    bool parseControlsGroup(yarp::os::Searchable &config);
    bool parseSelectedPositionControl(yarp::os::Searchable &config);
    bool parseSelectedVelocityControl(yarp::os::Searchable &config);
    bool parseSelectedTorqueControl(yarp::os::Searchable &config);
    bool parseSelectedCurrentPid(yarp::os::Searchable &config, bool currentPidisMandatory, eomcParser_pidInfo *cpids);
    bool parsePid_inPos_outPwm(yarp::os::Bottle &b_pid, std::string controlLaw);
    bool parsePid_inVel_outPwm(yarp::os::Bottle &b_pid, std::string controlLaw);
    bool parsePid_inTrq_outPwm(yarp::os::Bottle &b_pid, std::string controlLaw);
    bool parsePidPos_withInnerVelPid(yarp::os::Bottle &b_pid, std::string controlLaw);
    bool parsePidTrq_withInnerVelPid(yarp::os::Bottle &b_pid, std::string controlLaw);
    bool parsePidsGroup(yarp::os::Bottle& pidsGroup, yarp::dev::Pid myPid[], std::string prefix);
    bool getCorrectPidForEachJoint(eomcParser_pidInfo *ppids, eomcParser_pidInfo *vpids, eomcParser_trqPidInfo *tpids);
    bool parsePidUnitsType(yarp::os::Bottle &bPid, GenericControlUnitsType_t &unitstype);


    bool convert(yarp::os::ConstString const &fromstring, eOmc_jsetconstraint_t &jsetconstraint, bool& formaterror);
    bool convert(yarp::os::Bottle &bottle, std::vector<double> &matrix, bool &formaterror, int targetsize);

    //general utils functions
    bool extractGroup(yarp::os::Bottle &input, yarp::os::Bottle &out, const std::string &key1, const std::string &txt, int size);
    template <class T>
    bool checkAndSetVectorSize(std::vector<T> &vec, int size, const std::string &funcName)
    {
         if(size > vec.capacity())
        {
            yError() << "embObjMC BOARD " << _boardname << " in " <<  funcName.c_str() << ": try to insert " << size << "element in vector with " << vec.capacity() << " elements";
            return false;
        }

        vec.resize(size);
        return true;
    }

    ///////// DEBUG FUNCTIONS
    void debugUtil_printControlLaws(void);


public:
    mcParser(int numofjoints, std::string boardname);
    ~mcParser();

    bool parsePids(yarp::os::Searchable &config, eomcParser_pidInfo *ppids, eomcParser_pidInfo *vpids, eomcParser_trqPidInfo *tpids, eomcParser_pidInfo *cpids, bool currentPidisMandatory);
    bool parse2FocGroup(yarp::os::Searchable &config, eomc_twofocSpecificInfo *twofocinfo);
    //bool parseCurrentPid(yarp::os::Searchable &config, eomcParser_pidInfo *cpids);//deprecated
    bool parseJointsetCfgGroup(yarp::os::Searchable &config, std::vector<eomc_jointsSet> &jsets, std::vector<int> &jointtoset);
    bool parseTimeoutsGroup(yarp::os::Searchable &config, std::vector<eomc_timeouts_t> &timeouts, int defaultVelocityTimeout);
    bool parseCurrentLimits(yarp::os::Searchable &config, std::vector<eomc_motorCurrentLimits> &currLimits);
    bool parseJointsLimits(yarp::os::Searchable &config, std::vector<eomc_jointLimits> &jointsLimits);
    bool parseRotorsLimits(yarp::os::Searchable &config, std::vector<eomc_rotorLimits> &rotorsLimits);
    bool parseCouplingInfo(yarp::os::Searchable &config, eomc_couplingInfo_t &couplingInfo);
    bool parseMotioncontrolVersion(yarp::os::Searchable &config, int &version);
    bool parseBehaviourFalgs(yarp::os::Searchable &config, bool &useRawEncoderData, bool  &pwmIsLimited );
    bool isVerboseEnabled(yarp::os::Searchable &config);
    bool parseAxisInfo(yarp::os::Searchable &config, int axisMap[], std::vector<eomc_axisInfo_t> &axisInfo);
    bool parseEncoderFactor(yarp::os::Searchable &config, double encoderFactor[]);
    bool parseGearboxValues(yarp::os::Searchable &config, double gearbox_M2J[], double gearbox_E2J[]);
    bool parseMechanicalsFlags(yarp::os::Searchable &config, int useMotorSpeedFbk[]);
    bool parseImpedanceGroup(yarp::os::Searchable &config,std::vector<eomc_impedanceParameters> &impedance);
};

#endif // include guard

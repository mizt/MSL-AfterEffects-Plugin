#include "AEConfig.h"
#include "entry.h"
#include "AE_Effect.h"
#include "A.h"
#include "AE_EffectCB.h"
#include "AE_Macros.h"
#include "Param_Utils.h"

#include "MSL.h"

static PF_Err GlobalSetup(PF_InData *in_data, PF_OutData *out_data, PF_ParamDef *params[], PF_LayerDef    *output) {
    
    PF_Err err = PF_Err_NONE;
    
    out_data->my_version = PF_VERSION(2,0,0,PF_Stage_DEVELOP,1);
    out_data->out_flags = PF_OutFlag_I_EXPAND_BUFFER|PF_OutFlag_I_HAVE_EXTERNAL_DEPENDENCIES;
    out_data->out_flags2 = 0;

    in_data->sequence_data = out_data->sequence_data = nullptr;
    
    return err;
}

static PF_Err ParamsSetup(PF_InData *in_data, PF_OutData *out_data, PF_ParamDef *params[], PF_LayerDef *output) {
    
    return MSL::ParamsSetup(in_data,out_data);
 
}

static PF_Err SequenceSetup(PF_InData *in_data, PF_OutData *out_data, PF_ParamDef *params[], PF_LayerDef *output) {
    
    return PF_Err_NONE;
}

static PF_Err SequenceSetdown(PF_InData *in_data, PF_OutData *out_data, PF_ParamDef *params[], PF_LayerDef *output) {
    
    return PF_Err_NONE;
}

static PF_Err SequenceResetup(PF_InData *in_data, PF_OutData *out_data, PF_ParamDef *params[], PF_LayerDef *output) {
    
    return PF_Err_NONE;
}

static PF_Err Render(PF_InData *in_data, PF_OutData *out_data, PF_ParamDef *params[], PF_LayerDef    *output) {
    
    return MSL::Render(&params[0]->u.ld,output,params);
}

extern "C" { DllExport PF_Err EffectMain(PF_Cmd cmd, PF_InData *in_data, PF_OutData *out_data, PF_ParamDef *params[], PF_LayerDef *output) {
    PF_Err err = PF_Err_NONE;
    
    switch (cmd) {
        case PF_Cmd_GLOBAL_SETUP: err = GlobalSetup(in_data,out_data,params,output); break;
        case PF_Cmd_PARAMS_SETUP: err = ParamsSetup(in_data,out_data,params,output); break;
        case PF_Cmd_SEQUENCE_SETUP: err = SequenceSetup(in_data,out_data,params,output); break;
        case PF_Cmd_SEQUENCE_SETDOWN: err = SequenceSetdown(in_data,out_data,params,output); break;
        case PF_Cmd_SEQUENCE_RESETUP: err = SequenceResetup(in_data,out_data,params,output); break;
        case PF_Cmd_RENDER: err = Render(in_data,out_data,params,output); break;
    }
    return err;
}}

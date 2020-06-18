#import "AEConfig.h"
#import "entry.h"
#import "AE_Effect.h"
#import "A.h"
#import "AE_EffectCB.h"
#import "AE_Macros.h"
#import "Param_Utils.h"

#import "MSL.h"

extern "C" DllExport PF_Err EntryPointFunc(PF_Cmd cmd, PF_InData *in_data, PF_OutData *out_data, PF_ParamDef *params[], PF_LayerDef *output);

static PF_Err About(PF_InData *in_data, PF_OutData *out_data,PF_ParamDef *params[],PF_LayerDef *output) {
    return PF_Err_NONE;
}

static PF_Err GlobalSetup(PF_InData *in_data, PF_OutData *out_data, PF_ParamDef *params[], PF_LayerDef *output) {
    
    PF_Err err = PF_Err_NONE;
    
    out_data->my_version = PF_VERSION(2,0,0,PF_Stage_DEVELOP,1);
    
    out_data->out_flags = PF_OutFlag_I_EXPAND_BUFFER|PF_OutFlag_I_HAVE_EXTERNAL_DEPENDENCIES;
    out_data->out_flags2 =  PF_OutFlag2_NONE;
    
    in_data->sequence_data = out_data->sequence_data = nullptr;
    
    return err;
}

static PF_Err ParamsSetup(PF_InData *in_data,PF_OutData *out_data,PF_ParamDef *params[], PF_LayerDef *output) {
    
    PF_Err err = PF_Err_NONE;
    out_data->num_params = MSL_NUM_PARAMS;
    
    return err;
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

static PF_Err Render(PF_InData *in_data, PF_OutData *out_data, PF_ParamDef *params[], PF_LayerDef *output) {
    
    MSL::render(&params[0]->u.ld,output,params);
    
    return PF_Err_NONE;
}

DllExport PF_Err EntryPointFunc(PF_Cmd cmd, PF_InData *in_data, PF_OutData *out_data, PF_ParamDef *params[], PF_LayerDef *output) {
    
    PF_Err err = PF_Err_NONE;
    
    switch (cmd) {
        case PF_Cmd_ABOUT: err = About(in_data,out_data,params,output); break;
        case PF_Cmd_GLOBAL_SETUP: err = GlobalSetup(in_data,out_data,params,output); break;
        case PF_Cmd_PARAMS_SETUP: err = ParamsSetup(in_data,out_data,params,output); break;
        case PF_Cmd_SEQUENCE_SETUP: err = SequenceSetup(in_data,out_data,params,output); break;
        case PF_Cmd_SEQUENCE_SETDOWN: err = SequenceSetdown(in_data,out_data,params,output); break;
        case PF_Cmd_SEQUENCE_RESETUP: err = SequenceResetup(in_data,out_data,params,output); break;
        case PF_Cmd_RENDER: err = Render(in_data,out_data,params,output); break;
    }
    return err;
}

#import "AEConfig.h"
#import "AE_EffectVers.h"
#import "AE_General.r"

resource 'PiPL' (16000) {{
    Kind {AEEffect},
    Name {"MSL"},
    Category {"mizt"},
    CodeMacARM64 {"EffectMain"},
    AE_PiPL_Version {2,0},
    AE_Effect_Spec_Version {PF_PLUG_IN_VERSION,PF_PLUG_IN_SUBVERS},
    AE_Effect_Version {1048577},
    AE_Effect_Info_Flags {0},
    AE_Effect_Global_OutFlags {0x1000200},
    AE_Effect_Global_OutFlags_2 {0},
    AE_Effect_Match_Name {"mizt.MSL"},
    AE_Reserved_Info {0}
}};



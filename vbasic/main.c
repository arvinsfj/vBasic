//
//  main.c
//  vbasic
//
//  Created by arvin on 2017/7/14.
//  Copyright © 2017年 com.fuwo. All rights reserved.
//

#include "vbasic.h"

static const char prgm[] = \
"\
10 let k = 1\n\
11 if k then gosub 90\n\
20 for i = 1 to 9\n\
30 print \"hello\",,,,, i, i+1\n\
40 next i\n\
50 print \"end\"\n\
60 end\n\
70 print \"subroutine1\"\n\
80 return\n\
90 print \"subroutine2\"\n\
100 return\n\
";

int main(int argc, const char * argv[])
{
    vbasic_init(prgm);
    
    do {
        vbasic_run();
    } while (!vbasic_finished());
    
    return 0;
}

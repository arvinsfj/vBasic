//
//  vbasic.h
//  vbasic
//
//  Created by arvin on 2017/7/14.
//  Copyright © 2017年 com.fuwo. All rights reserved.
//

#ifndef vbasic_h
#define vbasic_h

void vbasic_init(const char* prgm);
void vbasic_run();
int  vbasic_finished();

//
int vbasic_get_variable(int varnum);
void vbasic_set_variable(int varnum, int value);

#endif /* vbasic_h */

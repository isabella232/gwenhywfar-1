/***************************************************************************
    begin       : Mon Feb 08 2021
    copyright   : (C) 2021 by Martin Preuss
    email       : martin@libchipcard.de

 ***************************************************************************
 *          Please see toplevel file COPYING for license details           *
 ***************************************************************************/

#ifndef GWBUILD_TRYCOMPILE_H
#define GWBUILD_TRYCOMPILE_H


#include "gwenbuild/gwenbuild.h"


int GWB_Tools_TryCompile(GWENBUILD *gwbuild, const char *testCode);

int GWB_Tools_CheckCompilerArgs(GWENBUILD *gwbuild, const char *compilerArgs);



#endif


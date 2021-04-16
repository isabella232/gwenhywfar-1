/***************************************************************************
    begin       : Mon Feb 08 2021
    copyright   : (C) 2021 by Martin Preuss
    email       : martin@libchipcard.de

 ***************************************************************************
 *          Please see toplevel file COPYING for license details           *
 ***************************************************************************/

#ifndef GWBUILD_PARSER_TARGET_H
#define GWBUILD_PARSER_TARGET_H


#include "gwenbuild/project.h"

#include <gwenhywfar/xml.h>



int GWB_ParseTarget(GWB_PROJECT *project, GWB_CONTEXT *currentContext, GWEN_XMLNODE *xmlNode);



#endif


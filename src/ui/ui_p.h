/***************************************************************************
 $RCSfile$
                             -------------------
    cvs         : $Id$
    begin       : Mon Feb 09 2004
    copyright   : (C) 2004 by Martin Preuss
    email       : martin@libchipcard.de

 ***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston,                 *
 *   MA  02111-1307  USA                                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef GWEN_UI_UI_P_H
#define GWEN_UI_UI_P_H


#include "ui_l.h"
#include <gwenhywfar/widget.h>


typedef struct GWEN_UI GWEN_UI;
struct GWEN_UI {
  GWEN_WIDGET_LIST *rootWidgets;
  GWEN_TYPE_UINT32 usage;
  GWEN_WIDGET *focusWidget;
  GWEN_EVENT_LIST *events;
  GWEN_EVENT_LIST *newEvents;
  GWEN_EVENT *currentEvent;
};


GWEN_UI_RESULT GWEN_UI_HandleEvents();



#endif




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

#ifndef GWEN_UI_WIDGET_H
#define GWEN_UI_WIDGET_H

#include <gwenhywfar/misc.h>
#include <gwenhywfar/inherit.h>

typedef struct GWEN_WIDGET GWEN_WIDGET;

GWEN_LIST_FUNCTION_DEFS(GWEN_WIDGET, GWEN_Widget)
GWEN_INHERIT_FUNCTION_DEFS(GWEN_WIDGET)

typedef enum {
  GWEN_WidgetColour_Default=1,
  GWEN_WidgetColour_Message,
  GWEN_WidgetColour_Unselected,
  GWEN_WidgetColour_Selected,
  GWEN_WidgetColour_Title
} GWEN_WIDGET_COLOUR;


#include <gwenhywfar/ui.h>
#include <gwenhywfar/event.h>


#define GWEN_WIDGET_FLAGS_PANEL      0x00000001
#define GWEN_WIDGET_FLAGS_KEYPAD     0x00000002
#define GWEN_WIDGET_FLAGS_SCROLL     0x00000004
#define GWEN_WIDGET_FLAGS_ACTIVE     0x00000008
#define GWEN_WIDGET_FLAGS_FOCUSABLE  0x00000010
#define GWEN_WIDGET_FLAGS_BORDER     0x00000020
#define GWEN_WIDGET_FLAGS_HASFOCUS   0x00000040
#define GWEN_WIDGET_FLAGS_NEEDCURSOR 0x00000080

#define GWEN_WIDGET_FLAGS_HCENTER   0x00000100
#define GWEN_WIDGET_FLAGS_VCENTER   0x00000200

#define GWEN_WIDGET_FLAGS_DEFAULT (\
  GWEN_WIDGET_FLAGS_KEYPAD |\
  GWEN_WIDGET_FLAGS_ACTIVE |\
  GWEN_WIDGET_FLAGS_FOCUSABLE \
  )


#define GWEN_WIDGET_ATT_ESC_CHAR  0xff
#define GWEN_WIDGET_ATT_NORMAL    0x00
#define GWEN_WIDGET_ATT_STANDOUT  0x01
#define GWEN_WIDGET_ATT_UNDERLINE 0x02
#define GWEN_WIDGET_ATT_REVERSE   0x04



typedef GWEN_UI_RESULT (*GWEN_WIDGET_EVENTHANDLER_FN)(GWEN_WIDGET *w,
                                                      GWEN_EVENT *e);


GWEN_WIDGET *GWEN_Widget_new(GWEN_WIDGET *parent,
                             GWEN_TYPE_UINT32 flags,
                             const char *name,
                             const char *text,
                             int x, int y, int width, int height);

void GWEN_Widget_free(GWEN_WIDGET *w);
void GWEN_Widget_Attach(GWEN_WIDGET *w);


const char *GWEN_Widget_GetName(const GWEN_WIDGET *w);
GWEN_TYPE_UINT32 GWEN_Widget_GetId(const GWEN_WIDGET *w);

GWEN_WIDGET *GWEN_Widget_GetParent(const GWEN_WIDGET *w);

int GWEN_Widget_GetX(const GWEN_WIDGET *w);
int GWEN_Widget_GetY(const GWEN_WIDGET *w);

int GWEN_Widget_GetWidth(const GWEN_WIDGET *w);
int GWEN_Widget_GetHeight(const GWEN_WIDGET *w);
const char *GWEN_Widget_GetText(const GWEN_WIDGET *w);

GWEN_TYPE_UINT32 GWEN_Widget_GetFlags(const GWEN_WIDGET *w);

GWEN_WIDGET_EVENTHANDLER_FN GWEN_Widget_GetEventHandler(const GWEN_WIDGET *w);
void GWEN_Widget_SetEventHandler(GWEN_WIDGET *w,
                                 GWEN_WIDGET_EVENTHANDLER_FN f);

int GWEN_Widget_SendEvent(GWEN_WIDGET *wRecipient,
                          GWEN_WIDGET *wSender,
                          GWEN_EVENT *e);

int GWEN_Widget_Redraw(GWEN_WIDGET *w);
int GWEN_Widget_SetText(GWEN_WIDGET *w, const char *t,
                        GWEN_EVENT_SETTEXT_MODE m);
int GWEN_Widget_Move(GWEN_WIDGET *w, int x, int y);
int GWEN_Widget_SetColour(GWEN_WIDGET *w, GWEN_WIDGET_COLOUR c);
GWEN_WIDGET_COLOUR GWEN_Widget_GetColour(const GWEN_WIDGET *w);

int GWEN_Widget_SetFocus(GWEN_WIDGET *w);
int GWEN_Widget_Destroy(GWEN_WIDGET *w);
int GWEN_Widget_Update(GWEN_WIDGET *w);
int GWEN_Widget_Refresh(GWEN_WIDGET *w);
int GWEN_Widget_Scroll(GWEN_WIDGET *w, int byX, int byY);
int GWEN_Widget_Scrolled(GWEN_WIDGET *w, int byX, int byY);
int GWEN_Widget_WriteAt(GWEN_WIDGET *w,
                        int x, int y,
                        const char *t, int len);
int GWEN_Widget_ChangeAtts(GWEN_WIDGET *w,
                           GWEN_TYPE_UINT32 atts,
                           int set);
int GWEN_Widget_Clear(GWEN_WIDGET *w, int x, int y, GWEN_EVENT_CLEAR_MODE m);
int GWEN_Widget_Highlight(GWEN_WIDGET *w, int x, int y, int len, int yes);



void GWEN_Widget_Dump(GWEN_WIDGET *w, int indent);


#endif






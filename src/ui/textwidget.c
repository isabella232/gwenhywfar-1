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

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif


#include "textwidget_p.h"
#include <gwenhywfar/debug.h>
#include <gwenhywfar/inherit.h>
#include <gwenhywfar/xml.h>
#include <gwenhywfar/text.h>
#include <gwenhywfar/bufferedio.h>
#include <gwenhywfar/bio_buffer.h>
#include <ctype.h>


#include <ncurses.h>



GWEN_LIST_FUNCTIONS(GWEN_TW_LINE, GWEN_TWLine)


GWEN_INHERIT(GWEN_WIDGET, GWEN_TEXTWIDGET);


GWEN_TW_LINE *GWEN_TWLine_new(GWEN_TYPE_UINT32 startAttributes,
                              int offset, const char *s) {
  GWEN_TW_LINE *l;
  int rv;

  GWEN_NEW_OBJECT(GWEN_TW_LINE, l);
  GWEN_LIST_INIT(GWEN_TW_LINE, l);

  rv=GWEN_TWLine_SetText(l, startAttributes,
                         offset, s);
  assert(rv==0);

  return l;
}



int GWEN_TWLine_SetText(GWEN_TW_LINE *l,
                        GWEN_TYPE_UINT32 startAttributes,
                        int offset, const char *s) {
  GWEN_TYPE_UINT32 endAttributes;

  assert(l);

  l->offset=offset;
  l->length=0;
  free(l->text);
  l->text=0;
  if (s) {
    const char *p;
    unsigned int i;
    int error;
    int lastWasEsc;
    unsigned int length;

    p=s;
    i=strlen(p);
    lastWasEsc=0;
    length=0;
    error=0;
    while(*p) {
      int c;

      if (*p=='%') {
        int j;

        if (i<3) {
          DBG_ERROR(0, "Bad string");
          error++;
          break;
        }
        i-=3;
        p++;
        j=toupper(*p)-'0';
        if (j>9)
          j-=7;
        if (j<0 || j>15) {
          DBG_ERROR(0, "Bad string");
          error++;
          break;
        }
        c=j<<4;
        p++;
        j=toupper(*p)-'0';
        if (j>9)
          j-=7;
        if (j<0 || j>15) {
          DBG_ERROR(0, "Bad string");
          error++;
          break;
        }
        c+=j;
      }
      else {
        c=*p;
        i--;
      }

      if (lastWasEsc) {
        endAttributes=c;
        lastWasEsc=0;
      }
      else {
        if (c==255)
          lastWasEsc=1;
        else {
          lastWasEsc=0;
          length++;
        }
      }
      p++;
    } /* while */
    if (lastWasEsc) {
      DBG_ERROR(0, "Bad string");
      error++;
    }

    assert(error==0);
    l->length=length;
    l->text=strdup(s);
    l->startAttributes=startAttributes;
    l->endAttributes=endAttributes;
  } /* if s */

  return 0;
}



void GWEN_TWLine_free(GWEN_TW_LINE *l){
  if (l) {
    GWEN_LIST_FINI(GWEN_TW_LINE, l);
    free(l->text);
    GWEN_FREE_OBJECT(l);
  }
}




GWEN_WIDGET *GWEN_TextWidget_new(GWEN_WIDGET *parent,
                                 GWEN_TYPE_UINT32 flags,
                                 const char *name,
                                 const char *text,
                                 int x, int y, int width, int height){
  GWEN_WIDGET *w;
  GWEN_TEXTWIDGET *win;

  w=GWEN_Widget_new(parent, flags, name, text, x, y, width, height);

  GWEN_NEW_OBJECT(GWEN_TEXTWIDGET, win);
  GWEN_INHERIT_SETDATA(GWEN_WIDGET, GWEN_TEXTWIDGET, w, win,
                       GWEN_TextWidget_freeData);
  win->lines=GWEN_TWLine_List_new();
  win->vwidth=width;
  win->vheight=height;

  win->previousHandler=GWEN_Widget_GetEventHandler(w);
  assert(win->previousHandler);
  GWEN_Widget_SetEventHandler(w, GWEN_TextWidget_EventHandler);
  return w;
}



void GWEN_TextWidget_freeData(void *bp, void *p) {
  GWEN_TEXTWIDGET *win;

  win=(GWEN_TEXTWIDGET*)p;
  GWEN_TWLine_List_free(win->lines);
  GWEN_FREE_OBJECT(win);
}



int GWEN_TextWidget_WriteLine(GWEN_WIDGET *w, int x, int y) {
  GWEN_TEXTWIDGET *win;
  GWEN_TW_LINE *l;
  int i;
  const char *p;

  assert(w);
  win=GWEN_INHERIT_GETDATA(GWEN_WIDGET, GWEN_TEXTWIDGET, w);
  assert(win);

  if (y<win->top) {
    DBG_NOTICE(0, "Line %d is invisible", y);
    return 0;
  }

  i=y;
  l=GWEN_TWLine_List_First(win->lines);
  while(l && i--) l=GWEN_TWLine_List_Next(l);
  if (!l) {
    DBG_NOTICE(0, "Line %d not found ", y);
    GWEN_Widget_Clear(w, 0, y-win->top, GWEN_EventClearMode_ToBottom);
    return 1;
  }

  DBG_NOTICE(0, "Found line %d", y);
  if (l->text[0]==0) {
    GWEN_Widget_Clear(w, 0, y-win->top, GWEN_EventClearMode_ToEOL);
    return 0;
  }

  p=l->text;
  if (p) {
    GWEN_TYPE_UINT32 startAttributes;
    unsigned int i;
    int error;
    int lastWasEsc;
    startAttributes=l->startAttributes;
    const char *startPos;
    int length;
    int pos;

    startPos=0;
    pos=0;
    i=strlen(p);
    lastWasEsc=0;
    length=0;
    while(*p) {
      int c;

      if (pos==win->left && !startPos)
        startPos=p;

      if (*p=='%') {
        int j;

        if (i<3) {
          DBG_ERROR(0, "Bad string");
          error++;
          break;
        }
        i-=3;
        p++;
        j=toupper(*p)-'0';
        if (j>9)
          j-=7;
        if (j<0 || j>15) {
          DBG_ERROR(0, "Bad string");
          error++;
          break;
        }
        c=j<<4;
        p++;
        j=toupper(*p)-'0';
        if (j>9)
          j-=7;
        if (j<0 || j>15) {
          DBG_ERROR(0, "Bad string");
          error++;
          break;
        }
        c+=j;
        p++;
      }
      else {
        c=*p;
        i--;
        p++;
      }

      if (lastWasEsc) {
        if (pos<win->left)
          startAttributes=c;
        lastWasEsc=0;
      }
      else {
        if (c==255)
          lastWasEsc=1;
        else {
          lastWasEsc=0;
          pos++;
          if (pos>=win->left) {
            length++;
            if (length==GWEN_Widget_GetWidth(w))
              break;
          }
        }
      }
    } /* while */
    if (lastWasEsc) {
      DBG_ERROR(0, "Bad string");
      error++;
    }
    if (!length) {
      // TODO: delete line
      return 0;
    }
    GWEN_Widget_ChangeAtts(w, startAttributes, 1);
    GWEN_Widget_WriteAt(w, 0, y-win->top, startPos, length);
  } /* if p */
  else {
    DBG_NOTICE(0, "No text");
  }
  return 0;
}



int GWEN_TextWidget_WriteArea(GWEN_WIDGET *w,
                              int x, int y,
                              int width, int height) {
  GWEN_TEXTWIDGET *win;
  int i;
  int rv;

  assert(w);
  win=GWEN_INHERIT_GETDATA(GWEN_WIDGET, GWEN_TEXTWIDGET, w);
  assert(win);

  /* validate */
  if (y<win->top) {
    i=win->top-y;
    if (height<=i)
      return 0;
    height-=i;
    y=win->top;
  }

  if (x<win->left) {
    i=win->left-x;
    if (width<=i)
      return 0;
    width-=i;
    x=win->left;
  }

  if (width>GWEN_Widget_GetWidth(w))
    width=GWEN_Widget_GetWidth(w);
  if (height>GWEN_Widget_GetHeight(w))
    height=GWEN_Widget_GetHeight(w);

  DBG_NOTICE(0, "x: %d y=%d Height: %d Width: %d (pos=%d)",
             x, y, height, width, win->pos);
  for (i=0; i<height; i++) {
    DBG_INFO(0, "Writing line %d (->%d)", i, y+i);
    rv=GWEN_TextWidget_WriteLine(w, x, y+i);
    if (rv==-1) {
      DBG_INFO(0, "Error writing line %d (->%d)", i, y+i);
      break;
    }
    else if (rv==1)
      break;
  } /* for */

  return 0;
}



void GWEN_TextWidget_SetVirtualSize(GWEN_WIDGET *w,
                                    int vwidth,
                                    int vheight){
  GWEN_TEXTWIDGET *win;

  assert(w);
  win=GWEN_INHERIT_GETDATA(GWEN_WIDGET, GWEN_TEXTWIDGET, w);
  assert(win);
  if (vwidth>=GWEN_Widget_GetWidth(w))
    win->vwidth=vwidth;
  if (vheight>=GWEN_Widget_GetHeight(w))
    win->vheight=vheight;
}



int GWEN_TextWidget_SetText(GWEN_WIDGET *w,
                            const char *text,
                            GWEN_EVENT_SETTEXT_MODE m) {
  GWEN_TEXTWIDGET *win;
  GWEN_XMLNODE *xmlNode;
  GWEN_XMLNODE *n;
  GWEN_BUFFEREDIO *bio;
  GWEN_BUFFER *buf;

  assert(w);
  win=GWEN_INHERIT_GETDATA(GWEN_WIDGET, GWEN_TEXTWIDGET, w);
  assert(win);

  if (m==GWEN_EventSetTextMode_Replace && !text) {
    GWEN_TWLine_List_Clear(win->lines);
    return 0;
  }

  buf=GWEN_Buffer_new(0, strlen(text), 0, 1);
  GWEN_Buffer_AppendString(buf, text);
  GWEN_Buffer_Rewind(buf);
  bio=GWEN_BufferedIO_Buffer2_new(buf, 1);
  GWEN_BufferedIO_SetReadBuffer(bio, 0, 256);
  xmlNode=GWEN_XMLNode_new(GWEN_XMLNodeTypeTag, "root");
  if (GWEN_XML_Parse(xmlNode, bio, GWEN_XML_FLAGS_DEFAULT)) {
    DBG_INFO(0, "here");
    GWEN_BufferedIO_free(bio);
    GWEN_XMLNode_free(xmlNode);
    return -1;
  }
  GWEN_BufferedIO_Close(bio);
  GWEN_BufferedIO_free(bio);

  if (m==GWEN_EventSetTextMode_Replace)
    GWEN_TWLine_List_Clear(win->lines);

  n=GWEN_XMLNode_GetFirstTag(xmlNode);
  if (n) {
    GWEN_BUFFER *lineBuf;
    int pos;
    GWEN_TW_LINE_LIST *ll;
    int rv;

    lineBuf=GWEN_Buffer_new(0, 256, 0, 1);
    pos=0;
    if (m!=GWEN_EventSetTextMode_Insert)
      ll=win->lines;
    else
      ll=GWEN_TWLine_List_new();
    rv=GWEN_TextWidget_ParseXMLTag(w,
                                   n,
                                   lineBuf,
                                   0,
                                   0,
                                   0,
                                   &pos,
                                   ll);
    if (rv) {
      if (m==GWEN_EventSetTextMode_Insert)
        GWEN_TWLine_List_free(ll);
      GWEN_Buffer_free(lineBuf);
      GWEN_XMLNode_free(xmlNode);
      return -1;
    }
  }

  return 0;
}



int GWEN_TextWidget_ParseXMLTag(GWEN_WIDGET *w,
                                GWEN_XMLNODE *n,
                                GWEN_BUFFER *buf,
                                GWEN_TYPE_UINT32 startAtts,
                                GWEN_TYPE_UINT32 currentAtts,
                                int indent,
                                int *pos,
                                GWEN_TW_LINE_LIST *ll) {
  const char *p;
  GWEN_TYPE_UINT32 newAtts;
  int newIndent;
  int rv;
  GWEN_TEXTWIDGET *win;
  GWEN_XMLNODE *nn;

  assert(w);
  win=GWEN_INHERIT_GETDATA(GWEN_WIDGET, GWEN_TEXTWIDGET, w);
  assert(win);

  DBG_NOTICE(0, "Parsing tag \"%s\" (%d)",
             GWEN_XMLNode_GetData(n), *pos);
  p=GWEN_XMLNode_GetData(n);
  if (!p) {
    DBG_ERROR(0, "Tag with no name ?");
    return -1;
  }

  newAtts=currentAtts;
  newIndent=indent;
  if (strcasecmp(p, "b")==0 ||
      strcasecmp(p, "strong")==0)
    newAtts|=GWEN_WIDGET_ATT_STANDOUT;
  else if (strcasecmp(p, "i")==0)
    newAtts|=GWEN_WIDGET_ATT_UNDERLINE;
  else if (strcasecmp(p, "p")==0)
    newIndent+=2;

  if (newAtts!=currentAtts) {
    GWEN_Buffer_AppendByte(buf, GWEN_WIDGET_ATT_ESC_CHAR);
    GWEN_Buffer_AppendByte(buf, newAtts & 0xff);
  }
  nn=GWEN_XMLNode_GetChild(n);
  if (nn) {
    rv=GWEN_TextWidget_ParseXMLSubNodes(w, nn, buf,
                                        startAtts, newAtts,
                                        newIndent,
                                        pos,
                                        ll);
  }
  else
    rv=1;
  if (newAtts!=currentAtts) {
    GWEN_Buffer_AppendByte(buf, GWEN_WIDGET_ATT_ESC_CHAR);
    GWEN_Buffer_AppendByte(buf, currentAtts & 0xff);
  }

  if (strcasecmp(p, "br")==0 ||
      strcasecmp(p, "p")==0 ||
      (strcasecmp(p, "gwen")==0 && GWEN_Buffer_GetUsedBytes(buf)) ||
      strcasecmp(p, "ul")==0 ||
      strcasecmp(p, "li")==0 ||
      strcasecmp(p, "ol")==0
     ) {
    GWEN_BUFFER *newbuf;
    GWEN_TW_LINE *l;

    newbuf=GWEN_Buffer_new(0, GWEN_Buffer_GetUsedBytes(buf)+128, 0, 1);
    GWEN_Buffer_Rewind(buf);

    if (GWEN_Text_EscapeToBufferTolerant2(buf, newbuf)) {
      DBG_INFO(0, "here");
      GWEN_Buffer_free(newbuf);
      return -1;
    }
    GWEN_Buffer_Rewind(newbuf);

    DBG_NOTICE(0, "New line");
    GWEN_Text_LogString(GWEN_Buffer_GetStart(newbuf),
                        GWEN_Buffer_GetUsedBytes(newbuf),
                        0,
                        GWEN_LoggerLevelNotice);
    l=GWEN_TWLine_new(startAtts, 0, GWEN_Buffer_GetStart(newbuf));
    GWEN_TWLine_List_Add(l, ll);
    GWEN_Buffer_Reset(buf);
    GWEN_Buffer_free(newbuf);
    *pos=0;
  }


  return rv;
}



int GWEN_TextWidget_ParseXMLSubNodes(GWEN_WIDGET *w,
                                     GWEN_XMLNODE *n,
                                     GWEN_BUFFER *buf,
                                     GWEN_TYPE_UINT32 startAtts,
                                     GWEN_TYPE_UINT32 currentAtts,
                                     int indent,
                                     int *pos,
                                     GWEN_TW_LINE_LIST *ll){
  const char *p;
  int rv;
  GWEN_TEXTWIDGET *win;

  assert(w);
  win=GWEN_INHERIT_GETDATA(GWEN_WIDGET, GWEN_TEXTWIDGET, w);
  assert(win);

  while(n) {
    int addSpace;

    switch(GWEN_XMLNode_GetType(n)) {
    case GWEN_XMLNodeTypeData:
      if (*pos==0) {
        int i;

        if (indent && currentAtts) {
          GWEN_Buffer_AppendByte(buf, GWEN_WIDGET_ATT_ESC_CHAR);
          GWEN_Buffer_AppendByte(buf, 0);
        }

        for (i=0; i<indent; i++) {
          if (GWEN_Widget_GetFlags(w) & GWEN_TEXTWIDGET_FLAGS_BREAKLINES) {
            if (*pos>=win->vwidth) {
              GWEN_BUFFER *newbuf;
              GWEN_TW_LINE *l;
  
              newbuf=GWEN_Buffer_new(0, GWEN_Buffer_GetUsedBytes(buf)+128,
                                     0, 1);
              GWEN_Buffer_Rewind(buf);
              if (GWEN_Text_EscapeToBufferTolerant2(buf, newbuf)) {
                DBG_INFO(0, "here");
                GWEN_Buffer_free(newbuf);
                return -1;
              }
              GWEN_Buffer_Rewind(newbuf);
  
              l=GWEN_TWLine_new(startAtts, 0, GWEN_Buffer_GetStart(newbuf));
              GWEN_TWLine_List_Add(l, ll);
              GWEN_Buffer_Reset(buf);
              GWEN_Buffer_free(newbuf);
              *pos=0;
            }
          }
          GWEN_Buffer_AppendByte(buf, ' ');
          (*pos)++;
        }
        if (indent && currentAtts) {
          GWEN_Buffer_AppendByte(buf, GWEN_WIDGET_ATT_ESC_CHAR);
          GWEN_Buffer_AppendByte(buf, currentAtts & 0xff);
        }
        addSpace=0;
      }
      else {
        addSpace=1;
      }

      p=GWEN_XMLNode_GetData(n);
      if (p) {
        if (addSpace) {
          GWEN_Buffer_AppendByte(buf, ' ');
          *(pos++);
        }

        while(*p) {
          if (GWEN_Widget_GetFlags(w) & GWEN_TEXTWIDGET_FLAGS_BREAKLINES) {
            if (*pos>=win->vwidth) {
              GWEN_BUFFER *newbuf;
              GWEN_TW_LINE *l;
          
              newbuf=GWEN_Buffer_new(0, GWEN_Buffer_GetUsedBytes(buf)+128,
                                     0, 1);
              GWEN_Buffer_Rewind(buf);
              if (GWEN_Text_EscapeToBufferTolerant2(buf, newbuf)) {
                DBG_INFO(0, "here");
                GWEN_Buffer_free(newbuf);
                return -1;
              }
              GWEN_Buffer_Rewind(newbuf);

              l=GWEN_TWLine_new(startAtts, 0, GWEN_Buffer_GetStart(newbuf));
              GWEN_TWLine_List_Add(l, ll);
              GWEN_Buffer_Reset(buf);
              GWEN_Buffer_free(newbuf);
              *pos=0;
            }
          }
          GWEN_Buffer_AppendByte(buf, *p);
          (*pos)++;
          p++;
        } /* while */
      }
      break;

    case GWEN_XMLNodeTypeTag:
      rv=GWEN_TextWidget_ParseXMLTag(w,
                                     n,
                                     buf,
                                     startAtts,
                                     currentAtts,
                                     indent,
                                     pos,
                                     ll);
      if (rv==-1) {
        DBG_INFO(0, "here");
        return rv;
      }
      else if (rv==1) {
      }
      break;
    default:
      break;
    }

    n=GWEN_XMLNode_Next(n);
  } /* while */

  return 0;
}




GWEN_UI_RESULT GWEN_TextWidget_EventHandler(GWEN_WIDGET *w, GWEN_EVENT *e) {
  GWEN_TEXTWIDGET *win;

  assert(w);
  win=GWEN_INHERIT_GETDATA(GWEN_WIDGET, GWEN_TEXTWIDGET, w);
  assert(win);
  assert(e);

  switch(GWEN_Event_GetType(e)) {
  case GWEN_EventType_SetText: {
    const char *p;
    GWEN_EVENT_SETTEXT_MODE m;

    DBG_NOTICE(0, "Event: SetText(%s)", GWEN_Widget_GetName(w));
    p=GWEN_EventSetText_GetText(e);
    m=GWEN_EventSetText_GetMode(e);
    if (p) {
      GWEN_TextWidget_SetText(w, p, m);
    }
    return GWEN_UIResult_Handled;
  }
  case GWEN_EventType_Draw: {
    DBG_NOTICE(0, "Event: Draw(%s)", GWEN_Widget_GetName(w));
    GWEN_TextWidget_WriteArea(w,
                              win->left,
                              win->top,
                              GWEN_Widget_GetWidth(w),
                              GWEN_Widget_GetHeight(w));
    if (GWEN_Widget_GetFlags(w) & GWEN_WIDGET_FLAGS_HASFOCUS) {
      GWEN_Widget_Highlight(w, 0, win->pos-win->top,
                            GWEN_Widget_GetWidth(w),
                            1);
    }
    GWEN_Widget_Update(w);
    return GWEN_UIResult_Handled;
  }

  case GWEN_EventType_Highlight: {
    int x, y;
    int maxc;
    int len;
    int hi;

    DBG_NOTICE(0, "Event: Highlight(%s)", GWEN_Widget_GetName(w));
    x=GWEN_EventHighlight_GetX(e);
    y=GWEN_EventHighlight_GetY(e);
    DBG_NOTICE(0, "Highlight %d/%d, top=%d", x, y, win->top);
    y+=win->top;
    hi=GWEN_EventHighlight_GetHi(e);
    win->previousHandler(w, e);
    if (!hi) {
      if (GWEN_Widget_GetFlags(w) & GWEN_WIDGET_FLAGS_BORDER) {
        x++;
        y++;
      }
      GWEN_TextWidget_WriteLine(w, x, y);
      GWEN_Widget_Refresh(w);
      return GWEN_UIResult_Handled;
    }
    else {
      return GWEN_UIResult_Handled;
    }
  }

  case GWEN_EventType_Scroll: {
    DBG_NOTICE(0, "Event: Scroll(%s)", GWEN_Widget_GetName(w));
    if (GWEN_EventScroll_GetTodo(e)) {
      int byX;
      int byY;

      DBG_NOTICE(0, "\nScrolling...\n");
      byX=GWEN_EventScroll_GetXBy(e);
      byY=GWEN_EventScroll_GetYBy(e);
      if (!byX && !byY)
        return GWEN_UIResult_Handled;

      if (byX) {
        /* handle horizontal scrolling */
        if (byX<0) {
          /* scroll backwards */
          if (win->left+byX<0)
            byX=-win->left;
        }
        else {
          /* scroll forward */
          if ((win->left+byX)>=win->vwidth)
            byX=win->vwidth-win->left-1;
        }
      }
      if (byY) {
        /* handle vertical scrolling */
        if (byY<0) {
          /* scroll backwards */
          if (win->top+byY<0)
            byY=-win->top;
        }
        else {
          /* scroll forward */
          DBG_NOTICE(0, "TOP: %d, byY: %d, vheight: %d",
                     win->top, byY, win->vheight);

          if (win->top+byY>=GWEN_TWLine_List_GetCount(win->lines)) {
            byY=GWEN_TWLine_List_GetCount(win->lines)-win->top-1;
            DBG_NOTICE(0, "byY corrected");
          }

          if ((win->top+byY)>=win->vheight) {
            byY=win->vheight-win->top-1;
            DBG_NOTICE(0, "Corrected byY: %d", byY);
          }
        }
      }
      if (byX)
        win->left+=byX;
      if (byY)
        win->top+=byY;
      if (byX || byY) {
        GWEN_Widget_Redraw(w);
        GWEN_Widget_Scrolled(w, byX, byY);
      }
      return GWEN_UIResult_Handled;
    }
    else {
      return win->previousHandler(w, e);
    }
  }

  case GWEN_EventType_Key: {
    int key;

    DBG_NOTICE(0, "Event: Key(%s)", GWEN_Widget_GetName(w));
    key=GWEN_EventKey_GetKey(e);
    if (key==KEY_DOWN) {
      if (GWEN_Widget_GetFlags(w) & GWEN_TEXTWIDGET_FLAGS_LINEMODE) {
        if (win->pos+1>
            win->offset+GWEN_TWLine_List_GetCount(win->lines)) {
          DBG_INFO(0, "Already at bottom of the list");
          beep();
        }
        else {
          /* new position is valid, select it */
          if ((win->pos+1-win->top)<win->vheight) {
            /* remove highlight */
            GWEN_Widget_Highlight(w, 0, win->pos-win->top,
                                  GWEN_Widget_GetWidth(w),
                                  0);
            win->pos++;
            if ((win->pos-win->top)>=GWEN_Widget_GetHeight(w)) {
              /* scroll up */
              GWEN_Widget_Scroll(w, 0, 1);
            }
            else
              GWEN_Widget_Highlight(w, 0, win->pos-win->top,
                                    GWEN_Widget_GetWidth(w),
                                    1);
          }
        }
      }
      else {
        /* not in linemode */
        if (win->top+1>=GWEN_TWLine_List_GetCount(win->lines)) {
          beep();
        }
        else {
          GWEN_Widget_Scroll(w, 0, 1);
        }
      }
      return GWEN_UIResult_Handled;
    }
    else if (key==KEY_UP) {
      if (GWEN_Widget_GetFlags(w) & GWEN_TEXTWIDGET_FLAGS_LINEMODE) {
        /* in line mode */
        if (win->pos) {
          GWEN_Widget_Highlight(w, 0, win->pos-win->top,
                                GWEN_Widget_GetWidth(w),
                                0);
          win->pos--;
          if (win->pos<win->top) {
            GWEN_Widget_Scroll(w, 0, -1);
          }
          else
            GWEN_Widget_Highlight(w, 0, win->pos-win->top,
                                  GWEN_Widget_GetWidth(w),
                                  1);
        }
        else {
          beep();
        }
      }
      else {
        if (win->top) {
          GWEN_Widget_Scroll(w, 0, -1);
        }
        else {
          beep();
        }
      }
      return GWEN_UIResult_Handled;
    }
    return win->previousHandler(w, e);
  }

  case GWEN_EventType_Focus: {
    GWEN_EVENT_FOCUS_TYPE ft;

    DBG_NOTICE(0, "Event: Focus(%s)", GWEN_Widget_GetName(w));
    win->previousHandler(w, e);
    ft=GWEN_EventFocus_GetFocusEventType(e);
    if (ft==GWEN_EventFocusType_Got)
      GWEN_Widget_Highlight(w, 0, win->pos-win->top,
                            GWEN_Widget_GetWidth(w),
                            1);
    else
      GWEN_Widget_Highlight(w, 0, win->pos-win->top,
                            GWEN_Widget_GetWidth(w),
                            0);
    return GWEN_UIResult_Handled;
  }
  default:
    return win->previousHandler(w, e);
  } /* switch */

  return win->previousHandler(w, e);
}










/***************************************************************************
    begin       : Fri Jan 22 2010
    copyright   : (C) 2010 by Martin Preuss
    email       : martin@libchipcard.de

 ***************************************************************************
 *          Please see toplevel file COPYING for license details           *
 ***************************************************************************/

#ifndef FOX16_GUI_DIALOG_L_HPP
#define FOX16_GUI_DIALOG_L_HPP

#include "cppdialog.hpp"
#include "fox16_gui.hpp"

#include <fx.h>

#include <gwenhywfar/dialog_be.h>
#include <gwenhywfar/widget_be.h>

#include <list>
#include <string>



class FOX16_GuiDialog: public FXObject, public CppDialog {
  FXDECLARE(FOX16_GuiDialog)

public:
  enum {
    ID_WIDGET_FIRST=1,
    ID_WIDGET_LAST=1000,
    ID_LAST
  };

  FOX16_GuiDialog(FOX16_Gui *gui, GWEN_DIALOG *dlg);
  virtual ~FOX16_GuiDialog();

  static FOX16_GuiDialog *getDialog(GWEN_DIALOG *dlg);

  long onSelCommand(FXObject *sender, FXSelector sel, void *ptr);
  long onSelChanged(FXObject *sender, FXSelector sel, void *ptr);
  long onSelKeyPress(FXObject *sender, FXSelector sel, void *ptr);
  long onSelKeyRelease(FXObject *sender, FXSelector sel, void *ptr);

  bool setup(FXWindow *parentWindow);

  int execute();
  int cont();

  int openDialog();
  int closeDialog();


  FXDialogBox *getMainWindow() { return _mainWidget;};

protected:
  FOX16_Gui *_gui;
  int _widgetCount;
  FXDialogBox *_mainWidget;

  FOX16_GuiDialog();

  FXWindow *setupTree(FXWindow *parentWindow, GWEN_WIDGET *w);

  void guiUpdate();

  virtual int setIntProperty(GWEN_WIDGET *w,
			     GWEN_DIALOG_PROPERTY prop,
			     int index,
			     int value,
			     int doSignal);

  virtual int getIntProperty(GWEN_WIDGET *w,
			     GWEN_DIALOG_PROPERTY prop,
			     int index,
			     int defaultValue);

  virtual int setCharProperty(GWEN_WIDGET *w,
			      GWEN_DIALOG_PROPERTY prop,
			      int index,
			      const char *value,
			      int doSignal);

  virtual const char *getCharProperty(GWEN_WIDGET *w,
				      GWEN_DIALOG_PROPERTY prop,
				      int index,
				      const char *defaultValue);

};




#endif /* FOX16_DIALOG_L_HPP */



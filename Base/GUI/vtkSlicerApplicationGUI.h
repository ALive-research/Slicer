/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkSlicerApplicationGUI.h,v $
  Date:      $Date: 2006/01/08 04:48:05 $
  Version:   $Revision: 1.45 $

=========================================================================auto=*/
// .NAME vtkSlicerApplicationGUI 
// .SECTION Description
// Main application GUI and mediator methods for slicer3.  

#ifndef __vtkSlicerApplicationGUI_h
#define __vtkSlicerApplicationGUI_h

#include "vtkSlicerBaseGUIWin32Header.h"
#include "vtkSlicerComponentGUI.h"

#include "vtkKWWindow.h"
#include "vtkKWMenuButton.h"
#include "vtkKWMenuButtonWithLabel.h"
#include "vtkKWFrame.h"
#include "vtkKWRenderWidget.h"

#include "vtkRenderWindowInteractor.h"

class vtkObject;
class vtkKWPushButton;
class vtkKWScale;
class vtkImplicitPlaneWidget;

// Description:
// This class implements Slicer's main Application GUI.
//
class VTK_SLICER_BASE_GUI_EXPORT vtkSlicerApplicationGUI : public vtkSlicerComponentGUI
{
 public:
    // Description:
    // Usual vtk class functions
    static vtkSlicerApplicationGUI* New (  );
    vtkTypeRevisionMacro ( vtkSlicerApplicationGUI, vtkSlicerComponentGUI );
    void PrintSelf ( ostream& os, vtkIndent indent );

    // Description:
    // These Get/Set methods for frames in the GUI panel.
    vtkGetObjectMacro ( MainViewer, vtkKWRenderWidget );
#if 0
//BTX
    // TODO: require change to KWWidgets
    vtkRenderWindowInteractor *GetRenderWindowInteractor() {
        return reinterpret_cast<vtkRenderWindowInteractor *> (this->MainViewer->GetInteractor());
    };
//ETX
#endif 

    vtkGetObjectMacro ( SlicerControlFrame, vtkKWFrame );
    vtkGetObjectMacro ( SliceControlFrame, vtkKWFrame );
    vtkGetObjectMacro ( ViewControlFrame, vtkKWFrame );
    vtkGetObjectMacro ( DefaultSlice0Frame, vtkKWFrame );
    vtkGetObjectMacro ( DefaultSlice1Frame, vtkKWFrame );
    vtkGetObjectMacro ( DefaultSlice2Frame, vtkKWFrame );

    vtkGetObjectMacro ( ModulesMenuButton, vtkKWMenuButton );
    vtkGetObjectMacro ( ModulesLabel, vtkKWLabel );
    vtkGetObjectMacro ( ModulesBack, vtkKWPushButton );
    vtkGetObjectMacro ( ModulesNext, vtkKWPushButton );    

    vtkGetObjectMacro ( HomeButton, vtkKWPushButton );
    vtkGetObjectMacro ( VolumesButton, vtkKWPushButton );
    vtkGetObjectMacro ( ModelsButton, vtkKWPushButton );
    vtkGetObjectMacro ( DataButton, vtkKWPushButton );
    vtkGetObjectMacro ( AlignmentsButton, vtkKWPushButton );

    vtkGetObjectMacro (ToggleAnnotationButton, vtkKWPushButton );
    vtkGetObjectMacro (ToggleFgBgButton, vtkKWPushButton );
    vtkGetObjectMacro (SliceFadeScale, vtkKWScale );
    vtkGetObjectMacro (SliceOpacityScale, vtkKWScale );

    // Description:
    // Get/Set the main slicer window.
    vtkGetObjectMacro ( MainSlicerWin, vtkKWWindow );

    // Description:
    // a Plane widget in the main window
    vtkGetObjectMacro ( PlaneWidget, vtkImplicitPlaneWidget );
    
    // Description:
    // This method builds Slicer's main GUI
    virtual void BuildGUI ( );

    // Description:
    // Add/Remove observers on widgets in Slicer's main GUI
    virtual void AddGUIObservers ( );
    virtual void RemoveGUIObservers ( );

    // Description:
    // Class's mediator methods for processing events invoked by
    // the Logic, MRML or GUI objects observed.
    virtual void ProcessLogicEvents ( vtkObject *caller, unsigned long event, void *callData );
    virtual void ProcessGUIEvents ( vtkObject *caller, unsigned long event, void *callData );
    virtual void ProcessMRMLEvents ( vtkObject *caller, unsigned long event, void *callData );
    
    // Description:
    // Methods describe behavior on startup and exit.
    virtual void Enter ( );
    virtual void Exit ( );
    
    // Description:
    // These methods configure and pack the Slicer Window
    virtual void ConfigureMainSlicerWindow ( );
    virtual void ConfigureMainViewerPanel ( );
    virtual void ConfigureSliceViewersPanel ( );
    virtual void ConfigureGUIPanel ( );
    
    // Description:
    // These methods populate the various GUI Panel frames
    virtual void BuildMainViewer ( );
    virtual void BuildLogoGUIPanel ( );
    virtual void BuildSlicerControlGUIPanel ( );
    virtual void BuildSliceControlGUIPanel ( );
    virtual void BuildViewControlGUIPanel ( );

    // Desrciption:
    // These methods delete widgets belonging to components of the Slicer Window
    virtual void DeleteGUIPanelWidgets ( );
    virtual void DeleteFrames ( );

    // Description:
    // Display Slicer's main window
    virtual void DisplayMainSlicerWindow ( );

 protected:
    vtkSlicerApplicationGUI ( );
    ~vtkSlicerApplicationGUI ( );

    // Description:
    // Widgets for the main Slicer UI panel    
    vtkKWFrame *LogoFrame;
    vtkKWFrame *SlicerControlFrame;
    vtkKWFrame *SliceControlFrame;
    vtkKWFrame *ViewControlFrame;
    vtkKWPushButton *HomeButton;
    vtkKWPushButton *DataButton;
    vtkKWPushButton *VolumesButton;
    vtkKWPushButton *ModelsButton;
    vtkKWPushButton *AlignmentsButton;
    vtkKWFrame *DefaultSlice0Frame;
    vtkKWFrame *DefaultSlice1Frame;
    vtkKWFrame *DefaultSlice2Frame;
    vtkKWRenderWidget *MainViewer;
    vtkImplicitPlaneWidget *PlaneWidget;
    // Description:
    // Widgets for the modules GUI panels
    vtkKWMenuButton *ModulesMenuButton;
    vtkKWLabel *ModulesLabel;
    vtkKWPushButton *ModulesBack;
    vtkKWPushButton *ModulesNext;
    //Description:
    // Widgets for the SliceControlFrame in the GUI
    vtkKWPushButton *ToggleAnnotationButton;
    vtkKWPushButton *ToggleFgBgButton;
    vtkKWScale *SliceFadeScale;
    vtkKWScale *SliceOpacityScale;

    // Description:
    // Main Slicer window
    vtkKWWindow *MainSlicerWin;
    
 private:
    vtkSlicerApplicationGUI ( const vtkSlicerApplicationGUI& ); // Not implemented.
    void operator = ( const vtkSlicerApplicationGUI& ); //Not implemented.
}; 

#endif

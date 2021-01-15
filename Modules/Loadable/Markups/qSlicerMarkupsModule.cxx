/*==============================================================================

  Program: 3D Slicer

  Portions (c) Copyright Brigham and Women's Hospital (BWH) All Rights Reserved.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

==============================================================================*/

// Qt includes
#include <QDebug>
#include <QSettings>

// Markups Logic includes
#include <vtkSlicerMarkupsLogic.h>

// MRMLDisplayableManager includes
#include <vtkMRMLSliceViewDisplayableManagerFactory.h>
#include <vtkMRMLThreeDViewDisplayableManagerFactory.h>

// QTGUI includes
#include <qSlicerApplication.h>
//#include <qSlicerCoreApplication.h>
#include <qSlicerIOManager.h>
#include <qSlicerNodeWriter.h>

// SubjectHierarchy Plugins includes
#include "qSlicerSubjectHierarchyPluginHandler.h"
#include "qSlicerSubjectHierarchyMarkupsPlugin.h"

// Markups includes
#include "qMRMLMarkupsROIWidget.h"
#include "qSlicerMarkupsAngleMeasurementsWidget.h"
#include "qSlicerMarkupsCurveSettingsWidget.h"
#include "qSlicerMarkupsModule.h"
#include "qSlicerMarkupsModuleWidget.h"
#include "qSlicerMarkupsReader.h"
#include "qSlicerMarkupsWriter.h"
#include "vtkMRMLMarkupsAngleNode.h"
#include "vtkMRMLMarkupsDisplayNode.h"
#include "vtkMRMLMarkupsClosedCurveNode.h"
#include "vtkMRMLMarkupsCurveNode.h"
#include "vtkMRMLMarkupsFiducialNode.h"
#include "vtkMRMLMarkupsLineNode.h"
#include "vtkMRMLMarkupsPlaneNode.h"
#include "vtkMRMLMarkupsROINode.h"
#include "vtkSlicerAngleWidget.h"
#include "vtkSlicerClosedCurveWidget.h"
#include "vtkSlicerCurveWidget.h"
#include "vtkSlicerLineWidget.h"
#include "vtkSlicerPlaneWidget.h"
#include "vtkSlicerPointsWidget.h"
#include "vtkSlicerROIWidget.h"

// DisplayableManager initialization
#include <vtkAutoInit.h>
VTK_MODULE_INIT(vtkSlicerMarkupsModuleMRMLDisplayableManager)

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_Markups
class qSlicerMarkupsModulePrivate
{
public:
  qSlicerMarkupsModulePrivate();
};

//-----------------------------------------------------------------------------
// qSlicerMarkupsModulePrivate methods

//-----------------------------------------------------------------------------
qSlicerMarkupsModulePrivate::qSlicerMarkupsModulePrivate() = default;

//-----------------------------------------------------------------------------
// qSlicerMarkupsModule methods

//-----------------------------------------------------------------------------
qSlicerMarkupsModule::qSlicerMarkupsModule(QObject* _parent)
  : Superclass(_parent)
  , d_ptr(new qSlicerMarkupsModulePrivate)
{
}

//-----------------------------------------------------------------------------
QStringList qSlicerMarkupsModule::categories()const
{
  return QStringList() << "" << "Informatics";
}

//-----------------------------------------------------------------------------
qSlicerMarkupsModule::~qSlicerMarkupsModule() = default;

//-----------------------------------------------------------------------------
QString qSlicerMarkupsModule::helpText()const
{
  QString help =
    "A module to create and manage markups in 2D and 3D."
    " Replaces the Annotations module for fiducials.";
  help += this->defaultDocumentationLink();
  return help;
}

//-----------------------------------------------------------------------------
QString qSlicerMarkupsModule::acknowledgementText()const
{
  return "This work was supported by NAMIC, NAC, and the Slicer Community.";
}

//-----------------------------------------------------------------------------
QStringList qSlicerMarkupsModule::contributors()const
{
  QStringList moduleContributors;
  moduleContributors << QString("Nicole Aucoin (BWH)");
  return moduleContributors;
}

//-----------------------------------------------------------------------------
QIcon qSlicerMarkupsModule::icon()const
{
  return QIcon(":/Icons/Markups.png");
}

//-----------------------------------------------------------------------------
void qSlicerMarkupsModule::setup()
{
  this->Superclass::setup();

  vtkSlicerMarkupsLogic *logic = vtkSlicerMarkupsLogic::SafeDownCast(this->logic());
  if (!logic)
    {
    qCritical() << Q_FUNC_INFO << ": cannot get Markups logic.";
    return;
    }

  // Register markups
  logic->SetMarkup(vtkMRMLMarkupsFiducialNode::New(), vtkSlicerPointsWidget::New());
  logic->SetMarkup(vtkMRMLMarkupsLineNode::New(), vtkSlicerLineWidget::New());
  logic->SetMarkup(vtkMRMLMarkupsAngleNode::New(), vtkSlicerAngleWidget::New());
  logic->SetMarkup(vtkMRMLMarkupsCurveNode::New(), vtkSlicerCurveWidget::New());
  logic->SetMarkup(vtkMRMLMarkupsClosedCurveNode::New(), vtkSlicerClosedCurveWidget::New());
  logic->SetMarkup(vtkMRMLMarkupsPlaneNode::New(), vtkSlicerPlaneWidget::New());
  logic->SetMarkup(vtkMRMLMarkupsROINode::New(), vtkSlicerROIWidget::New());

  // Register displayable managers (same displayable manager handles both slice and 3D views)
  vtkMRMLSliceViewDisplayableManagerFactory::GetInstance()->RegisterDisplayableManager("vtkMRMLMarkupsDisplayableManager");
  vtkMRMLThreeDViewDisplayableManagerFactory::GetInstance()->RegisterDisplayableManager("vtkMRMLMarkupsDisplayableManager");

  // Register IO
  qSlicerIOManager* ioManager = qSlicerApplication::application()->ioManager();
  qSlicerMarkupsReader *markupsReader = new qSlicerMarkupsReader(vtkSlicerMarkupsLogic::SafeDownCast(this->logic()), this);
  ioManager->registerIO(markupsReader);
  ioManager->registerIO(new qSlicerMarkupsWriter(this));

  // settings
  /*
  if (qSlicerApplication::application())
    {
    qSlicerMarkupsSettingsPanel* panel =
      new qSlicerMarkupsSettingsPanel;
    qSlicerApplication::application()->settingsDialog()->addPanel(
      "Markups", panel);
    panel->setMarkupsLogic(vtkSlicerMarkupsLogic::SafeDownCast(this->logic()));
    }
  */

  // Register Subject Hierarchy core plugins
  qSlicerSubjectHierarchyPluginHandler::instance()->registerPlugin(new qSlicerSubjectHierarchyMarkupsPlugin());
}

//-----------------------------------------------------------------------------
qSlicerAbstractModuleRepresentation* qSlicerMarkupsModule::createWidgetRepresentation()
{
  // Create and configure module widget.
  qSlicerMarkupsModuleWidget* moduleWidget = new qSlicerMarkupsModuleWidget();
  moduleWidget->setCreateMarkupsButtonsColumns(6);

  // Register markups
  moduleWidget->setMarkup("Fiducial");
  moduleWidget->setMarkup("Line");
  moduleWidget->setMarkup("Angle", new qSlicerMarkupsAngleMeasurementsWidget);
  moduleWidget->setMarkup("Curve", new qSlicerMarkupsCurveSettingsWidget);
  moduleWidget->setMarkup("ClosedCurve");
  moduleWidget->setMarkup("Plane");
  moduleWidget->setMarkup("ROI", new qMRMLMarkupsROIWidget);

  // Set the number of columns for the grid of "add markups buttons" to the number of markups
  // regitered in this module.
  moduleWidget->setCreateMarkupsButtonsColumns(7);

  return moduleWidget;
}

//-----------------------------------------------------------------------------
vtkMRMLAbstractLogic* qSlicerMarkupsModule::createLogic()
{
  return vtkSlicerMarkupsLogic::New();
}

//-----------------------------------------------------------------------------
QStringList qSlicerMarkupsModule::associatedNodeTypes() const
{
  return QStringList()
    << "vtkMRMLAnnotationFiducialNode"
    << "vtkMRMLMarkupsDisplayNode"
    << "vtkMRMLMarkupsFiducialNode"
    << "vtkMRMLMarkupsLineNode"
    << "vtkMRMLMarkupsAngleNode"
    << "vtkMRMLMarkupsCurveNode"
    << "vtkMRMLMarkupsClosedCurveNode"
    << "vtkMRMLMarkupsPlaneNode"
    << "vtkMRMLMarkupsROINode"
    << "vtkMRMLMarkupsFiducialStorageNode"
    << "vtkMRMLMarkupsJsonStorageNode";
}

//-----------------------------------------------------------------------------
void qSlicerMarkupsModule::setMRMLScene(vtkMRMLScene* scene)
{
  this->Superclass::setMRMLScene(scene);
  vtkSlicerMarkupsLogic* logic = vtkSlicerMarkupsLogic::SafeDownCast(this->logic());
  if (!logic)
    {
    qCritical() << Q_FUNC_INFO << " failed: logic is invalid";
    return;
    }
  // Update default view nodes from settings
  this->readDefaultMarkupsDisplaySettings(logic->GetDefaultMarkupsDisplayNode());
  this->writeDefaultMarkupsDisplaySettings(logic->GetDefaultMarkupsDisplayNode());
}

//-----------------------------------------------------------------------------
void qSlicerMarkupsModule::readDefaultMarkupsDisplaySettings(vtkMRMLMarkupsDisplayNode* markupsDisplayNode)
{
  if (!markupsDisplayNode)
    {
    qCritical() << Q_FUNC_INFO << " failed: markupsDisplayNode is invalid";
    return;
    }
  QSettings settings;
  if (settings.contains("Markups/GlyphType"))
    {
    markupsDisplayNode->SetGlyphType(vtkMRMLMarkupsDisplayNode::GetGlyphTypeFromString(
      settings.value("Markups/GlyphType").toString().toUtf8()));
    }
  if (settings.contains("Markups/SelectedColor"))
    {
    QVariant variant = settings.value("Markups/SelectedColor");
    QColor qcolor = variant.value<QColor>();
    markupsDisplayNode->SetSelectedColor(qcolor.redF(), qcolor.greenF(), qcolor.blueF());
    }
  if (settings.contains("Markups/UnselectedColor"))
    {
    QVariant variant = settings.value("Markups/UnselectedColor");
    QColor qcolor = variant.value<QColor>();
    markupsDisplayNode->SetColor(qcolor.redF(), qcolor.greenF(), qcolor.blueF());
    }
  if (settings.contains("Markups/ActiveColor"))
    {
    QVariant variant = settings.value("Markups/ActiveColor");
    QColor qcolor = variant.value<QColor>();
    markupsDisplayNode->SetColor(qcolor.redF(), qcolor.greenF(), qcolor.blueF());
    }
  if (settings.contains("Markups/UseGlyphScale"))
    {
    markupsDisplayNode->SetUseGlyphScale(settings.value("Markups/UseGlyphScale").toBool());
    }
  if (settings.contains("Markups/GlyphScale"))
    {
    markupsDisplayNode->SetGlyphScale(settings.value("Markups/GlyphScale").toDouble());
    }
  if (settings.contains("Markups/GlyphSize"))
    {
    markupsDisplayNode->SetGlyphSize(settings.value("Markups/GlyphSize").toDouble());
    }
  if (settings.contains("Markups/TextScale"))
    {
    markupsDisplayNode->SetTextScale(settings.value("Markups/TextScale").toDouble());
    }
  if (settings.contains("Markups/Opacity"))
    {
    markupsDisplayNode->SetOpacity(settings.value("Markups/Opacity").toDouble());
    }
  if (settings.contains("Markups/SliceProjection"))
    {
    markupsDisplayNode->SetSliceProjection(settings.value("Markups/SliceProjection").toBool());
    }
  if (settings.contains("Markups/SliceProjectionColor"))
    {
    QVariant variant = settings.value("Markups/SliceProjectionColor");
    QColor qcolor = variant.value<QColor>();
    markupsDisplayNode->SetSliceProjectionColor(qcolor.redF(), qcolor.greenF(), qcolor.blueF());
    }
  if (settings.contains("Markups/SliceProjectionOpacity"))
    {
    markupsDisplayNode->SetSliceProjectionOpacity(settings.value("Markups/SliceProjectionOpacity").toDouble());
    }
}

//-----------------------------------------------------------------------------
void qSlicerMarkupsModule::writeDefaultMarkupsDisplaySettings(vtkMRMLMarkupsDisplayNode* markupsDisplayNode)
{
  if (!markupsDisplayNode)
    {
    qCritical() << Q_FUNC_INFO << " failed: markupsDisplayNode is invalid";
    return;
    }
  QSettings settings;

  settings.setValue("Markups/GlyphType", vtkMRMLMarkupsDisplayNode::GetGlyphTypeAsString(
    markupsDisplayNode->GetGlyphType()));

  double* color = markupsDisplayNode->GetSelectedColor();
  settings.setValue("Markups/SelectedColor", QColor::fromRgbF(color[0], color[1], color[2]));
  color = markupsDisplayNode->GetColor();
  settings.setValue("Markups/UnselectedColor", QColor::fromRgbF(color[0], color[1], color[2]));
  color = markupsDisplayNode->GetActiveColor();
  settings.setValue("Markups/ActiveColor", QColor::fromRgbF(color[0], color[1], color[2]));
  settings.setValue("Markups/UseGlyphScale", markupsDisplayNode->GetUseGlyphScale());
  settings.setValue("Markups/GlyphScale", markupsDisplayNode->GetGlyphScale());
  settings.setValue("Markups/GlyphSize", markupsDisplayNode->GetGlyphSize());
  settings.setValue("Markups/TextScale", markupsDisplayNode->GetTextScale());
  settings.setValue("Markups/Opacity", markupsDisplayNode->GetOpacity());

  settings.setValue("Markups/SliceProjection", markupsDisplayNode->GetSliceProjection());
  color = markupsDisplayNode->GetSliceProjectionColor();
  settings.setValue("Markups/SliceProjectionColor", QColor::fromRgbF(color[0], color[1], color[2]));
  settings.setValue("Markups/SliceProjectionOpacity", markupsDisplayNode->GetSliceProjectionOpacity());
}

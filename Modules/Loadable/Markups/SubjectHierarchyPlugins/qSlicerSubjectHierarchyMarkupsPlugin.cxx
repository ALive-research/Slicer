/*==============================================================================

  Program: 3D Slicer

  Copyright (c) Laboratory for Percutaneous Surgery (PerkLab)
  Queen's University, Kingston, ON, Canada. All Rights Reserved.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  This file was originally developed by Csaba Pinter, PerkLab, Queen's University
  and was supported through the Applied Cancer Research Unit program of Cancer Care
  Ontario with funds provided by the Ontario Ministry of Health and Long-Term Care

==============================================================================*/

// SubjectHierarchy MRML includes
#include "vtkMRMLSubjectHierarchyNode.h"
#include "vtkMRMLSubjectHierarchyConstants.h"

// SubjectHierarchy Plugins includes
#include "qSlicerSubjectHierarchyPluginHandler.h"
#include "qSlicerSubjectHierarchyMarkupsPlugin.h"
#include "qSlicerSubjectHierarchyDefaultPlugin.h"

// Terminologies includes
#include "qSlicerTerminologyItemDelegate.h"
#include "vtkSlicerTerminologiesModuleLogic.h"

// MRML markups includes
#include "vtkMRMLMarkupsFiducialNode.h"

// MRML widgets includes
#include "qMRMLNodeComboBox.h"

// MRML includes
#include <vtkMRMLDisplayableNode.h>
#include <vtkMRMLDisplayNode.h>
#include <vtkMRMLMarkupsDisplayNode.h>
#include <vtkMRMLMarkupsROIDisplayNode.h>
#include <vtkMRMLMarkupsNode.h>
#include <vtkMRMLScene.h>

//Logic includes
#include <vtkSlicerMarkupsLogic.h>

// vtkSegmentationCore includes
#include <vtkSegment.h>

// VTK includes
#include <vtkObjectFactory.h>
#include <vtkSmartPointer.h>

// Qt includes
#include <QAction>
#include <QDebug>
#include <QInputDialog>
#include <QMenu>
#include <QStandardItem>

// Slicer includes
#include "qSlicerAbstractModuleWidget.h"
#include "qSlicerApplication.h"

//-----------------------------------------------------------------------------
const char* INTERACTION_HANDLE_TYPE_PROPERTY = "InteractionHandleType";

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_SubjectHierarchy_Plugins
class qSlicerSubjectHierarchyMarkupsPluginPrivate: public QObject
{
  Q_DECLARE_PUBLIC(qSlicerSubjectHierarchyMarkupsPlugin);
protected:
  qSlicerSubjectHierarchyMarkupsPlugin* const q_ptr;
public:
  qSlicerSubjectHierarchyMarkupsPluginPrivate(qSlicerSubjectHierarchyMarkupsPlugin& object);
  ~qSlicerSubjectHierarchyMarkupsPluginPrivate() override;
  void init();

public:
  QAction* RenamePointAction;
  QAction* DeletePointAction;
  QAction* ToggleSelectPointAction;
  QAction* ToggleCurrentItemHandleInteractive;
  QAction* ToggleHandleInteractive;

  QMenu* CurrentItemHandleVisibilityMenu;
  QAction* CurrentItemHandleVisibilityAction;
  QAction* ToggleCurrentItemTranslateHandleVisible;
  QAction* ToggleCurrentItemRotateHandleVisible;
  QAction* ToggleCurrentItemScaleHandleVisible;

  QMenu* HandleVisibilityMenu;
  QAction* HandleVisibilityAction;
  QAction* ToggleTranslateHandleVisible;
  QAction* ToggleRotateHandleVisible;
  QAction* ToggleScaleHandleVisible;

  QVariantMap ViewMenuEventData;
};

//-----------------------------------------------------------------------------
// qSlicerSubjectHierarchyMarkupsPluginPrivate methods

//-----------------------------------------------------------------------------
qSlicerSubjectHierarchyMarkupsPluginPrivate::qSlicerSubjectHierarchyMarkupsPluginPrivate(qSlicerSubjectHierarchyMarkupsPlugin& object)
: q_ptr(&object)
, RenamePointAction(nullptr)
, DeletePointAction(nullptr)
, ToggleSelectPointAction(nullptr)
, ToggleCurrentItemHandleInteractive(nullptr)
, ToggleHandleInteractive(nullptr)
{
}

//------------------------------------------------------------------------------
void qSlicerSubjectHierarchyMarkupsPluginPrivate::init()
{
  Q_Q(qSlicerSubjectHierarchyMarkupsPlugin);

  this->RenamePointAction = new QAction("Rename point...", q);
  this->RenamePointAction->setObjectName("RenamePointAction");
  QObject::connect(this->RenamePointAction, SIGNAL(triggered()), q, SLOT(renamePoint()));

  this->DeletePointAction = new QAction("Delete point", q);
  this->DeletePointAction->setObjectName("DeletePointAction");
  QObject::connect(this->DeletePointAction, SIGNAL(triggered()), q, SLOT(deletePoint()));

  this->ToggleSelectPointAction = new QAction("Toggle select point", q);
  this->ToggleSelectPointAction->setObjectName("ToggleSelectPointAction");
  QObject::connect(this->ToggleSelectPointAction, SIGNAL(triggered()), q, SLOT(toggleSelectPoint()));

  this->ToggleCurrentItemHandleInteractive = new QAction("Interaction");
  this->ToggleCurrentItemHandleInteractive->setObjectName("ToggleCurrentItemHandleInteractive");
  this->ToggleCurrentItemHandleInteractive->setCheckable(true);
  QObject::connect(this->ToggleCurrentItemHandleInteractive, SIGNAL(triggered()), q, SLOT(toggleCurrentItemHandleInteractive()));

  this->ToggleHandleInteractive = new QAction("Interaction");
  this->ToggleHandleInteractive->setObjectName("ToggleHandleInteractive");
  this->ToggleHandleInteractive->setCheckable(true);
  QObject::connect(this->ToggleHandleInteractive, SIGNAL(triggered()), q, SLOT(toggleHandleInteractive()));

  this->ToggleCurrentItemTranslateHandleVisible = new QAction("Translate");
  this->ToggleCurrentItemTranslateHandleVisible->setCheckable(true);
  this->ToggleCurrentItemTranslateHandleVisible->setProperty(INTERACTION_HANDLE_TYPE_PROPERTY, vtkMRMLMarkupsDisplayNode::ComponentTranslationHandle);
  QObject::connect(this->ToggleCurrentItemTranslateHandleVisible, SIGNAL(triggered()), q, SLOT(toggleCurrentItemHandleTypeVisibility()));

  this->ToggleCurrentItemRotateHandleVisible = new QAction("Rotate");
  this->ToggleCurrentItemRotateHandleVisible->setCheckable(true);
  this->ToggleCurrentItemRotateHandleVisible->setProperty(INTERACTION_HANDLE_TYPE_PROPERTY, vtkMRMLMarkupsDisplayNode::ComponentRotationHandle);
  QObject::connect(this->ToggleCurrentItemRotateHandleVisible, SIGNAL(triggered()), q, SLOT(toggleCurrentItemHandleTypeVisibility()));

  this->ToggleCurrentItemScaleHandleVisible = new QAction("Scale");
  this->ToggleCurrentItemScaleHandleVisible->setCheckable(true);
  this->ToggleCurrentItemScaleHandleVisible->setProperty(INTERACTION_HANDLE_TYPE_PROPERTY, vtkMRMLMarkupsDisplayNode::ComponentScaleHandle);
  QObject::connect(this->ToggleCurrentItemScaleHandleVisible, SIGNAL(triggered()), q, SLOT(toggleCurrentItemHandleTypeVisibility()));

  this->CurrentItemHandleVisibilityMenu = new QMenu();

  this->CurrentItemHandleVisibilityMenu->addAction(this->ToggleCurrentItemTranslateHandleVisible);
  this->CurrentItemHandleVisibilityMenu->addAction(this->ToggleCurrentItemRotateHandleVisible);
  this->CurrentItemHandleVisibilityMenu->addAction(this->ToggleCurrentItemScaleHandleVisible);

  this->CurrentItemHandleVisibilityAction = new QAction("Interaction options");
  this->CurrentItemHandleVisibilityAction->setMenu(this->CurrentItemHandleVisibilityMenu);

  this->ToggleTranslateHandleVisible = new QAction("Translate");
  this->ToggleTranslateHandleVisible->setCheckable(true);
  this->ToggleTranslateHandleVisible->setProperty(INTERACTION_HANDLE_TYPE_PROPERTY, vtkMRMLMarkupsDisplayNode::ComponentTranslationHandle);
  QObject::connect(this->ToggleTranslateHandleVisible, SIGNAL(triggered()), q, SLOT(toggleHandleTypeVisibility()));

  this->ToggleRotateHandleVisible = new QAction("Rotate");
  this->ToggleRotateHandleVisible->setCheckable(true);
  this->ToggleRotateHandleVisible->setProperty(INTERACTION_HANDLE_TYPE_PROPERTY, vtkMRMLMarkupsDisplayNode::ComponentRotationHandle);
  QObject::connect(this->ToggleRotateHandleVisible, SIGNAL(triggered()), q, SLOT(toggleHandleTypeVisibility()));

  this->ToggleScaleHandleVisible = new QAction("Scale");
  this->ToggleScaleHandleVisible->setCheckable(true);
  this->ToggleScaleHandleVisible->setProperty(INTERACTION_HANDLE_TYPE_PROPERTY, vtkMRMLMarkupsDisplayNode::ComponentScaleHandle);
  QObject::connect(this->ToggleScaleHandleVisible, SIGNAL(triggered()), q, SLOT(toggleHandleTypeVisibility()));

  this->HandleVisibilityMenu = new QMenu();
  this->HandleVisibilityMenu->addAction(this->ToggleTranslateHandleVisible);
  this->HandleVisibilityMenu->addAction(this->ToggleRotateHandleVisible);
  this->HandleVisibilityMenu->addAction(this->ToggleScaleHandleVisible);

  this->HandleVisibilityAction = new QAction("Interaction options");
  this->HandleVisibilityAction->setMenu(this->HandleVisibilityMenu);
}

//-----------------------------------------------------------------------------
qSlicerSubjectHierarchyMarkupsPluginPrivate::~qSlicerSubjectHierarchyMarkupsPluginPrivate() = default;

//-----------------------------------------------------------------------------
// qSlicerSubjectHierarchyMarkupsPlugin methods

//-----------------------------------------------------------------------------
qSlicerSubjectHierarchyMarkupsPlugin::qSlicerSubjectHierarchyMarkupsPlugin(QObject* parent)
 : Superclass(parent)
 , d_ptr( new qSlicerSubjectHierarchyMarkupsPluginPrivate(*this) )
{
  this->m_Name = QString("Markups");

  Q_D(qSlicerSubjectHierarchyMarkupsPlugin);
  d->init();
}

//-----------------------------------------------------------------------------
qSlicerSubjectHierarchyMarkupsPlugin::~qSlicerSubjectHierarchyMarkupsPlugin() = default;

//-----------------------------------------------------------------------------
double qSlicerSubjectHierarchyMarkupsPlugin::canAddNodeToSubjectHierarchy(
  vtkMRMLNode* node, vtkIdType parentItemID/*=vtkMRMLSubjectHierarchyNode::INVALID_ITEM_ID*/)const
{
  Q_UNUSED(parentItemID);
  if (!node)
    {
    qCritical() << Q_FUNC_INFO << ": Input node is NULL";
    return 0.0;
    }

  vtkSlicerApplicationLogic* appLogic = qSlicerApplication::application()->applicationLogic();
  if (!appLogic)
    {
    qCritical() << Q_FUNC_INFO << ": cannot get application logic";
    return 0.0;
    }

  vtkSlicerMarkupsLogic* markupsLogic = vtkSlicerMarkupsLogic::SafeDownCast(appLogic->GetModuleLogic("Markups"));
  if (!markupsLogic)
    {
    qCritical() << Q_FUNC_INFO << ": could not get the Markups module logic.";
    return 0.0;
    }

  bool registered = markupsLogic->GetWidgetByMarkupsNodeClass(node->GetClassName()) ? true : false;
  if (registered)
    {
    // Item is a registered markup
    return 0.5;
    }

  // Item is not a registered markup
  return 0.0;
}

//---------------------------------------------------------------------------
double qSlicerSubjectHierarchyMarkupsPlugin::canOwnSubjectHierarchyItem(vtkIdType itemID)const
{
  if (itemID == vtkMRMLSubjectHierarchyNode::INVALID_ITEM_ID)
    {
    qCritical() << Q_FUNC_INFO << ": Invalid input item";
    return 0.0;
    }
  vtkMRMLSubjectHierarchyNode* shNode = qSlicerSubjectHierarchyPluginHandler::instance()->subjectHierarchyNode();
  if (!shNode)
    {
    qCritical() << Q_FUNC_INFO << ": Failed to access subject hierarchy node";
    return 0.0;
    }

  // Markup
  vtkMRMLNode* associatedNode = shNode->GetItemDataNode(itemID);
  if (!associatedNode)
    {
    //NOTE: should there be a warning here?
    return 0.0;
    }

  vtkSlicerApplicationLogic* appLogic = qSlicerApplication::application()->applicationLogic();
  if (!appLogic)
    {
    qCritical() << Q_FUNC_INFO << ": cannot get application logic";
    return 0.0;
    }

  vtkSlicerMarkupsLogic* markupsLogic = vtkSlicerMarkupsLogic::SafeDownCast(appLogic->GetModuleLogic("Markups"));
  if (!markupsLogic)
    {
    qCritical() << Q_FUNC_INFO << ": could not get the Markups module logic.";
    return 0.0;
    }

  bool registered = markupsLogic->GetWidgetByMarkupsNodeClass(associatedNode->GetClassName()) ? true : false;
  if (registered)
    {
    // Item is a registered markup
    return 0.5;
    }
  // Item is not a registered markup
  return 0.0;
}

//---------------------------------------------------------------------------
const QString qSlicerSubjectHierarchyMarkupsPlugin::roleForPlugin()const
{
  return "Markup";
}

//---------------------------------------------------------------------------
QIcon qSlicerSubjectHierarchyMarkupsPlugin::icon(vtkIdType itemID)
{
  if (itemID == vtkMRMLSubjectHierarchyNode::INVALID_ITEM_ID)
    {
    qCritical() << Q_FUNC_INFO << ": Invalid input item";
    return QIcon();
    }

  Q_D(qSlicerSubjectHierarchyMarkupsPlugin);

  if (!this->canOwnSubjectHierarchyItem(itemID))
    {
    // Item unknown by plugin
    return QIcon();
    }
  vtkMRMLSubjectHierarchyNode* shNode = qSlicerSubjectHierarchyPluginHandler::instance()->subjectHierarchyNode();
  if (!shNode)
    {
    return QIcon();
    }
  vtkMRMLNode* node = shNode->GetItemDataNode(itemID);
  if (!node)
    {
    return QIcon();
    }

  vtkMRMLMarkupsNode *markupsNode = vtkMRMLMarkupsNode::SafeDownCast(node);
  if (markupsNode == nullptr)
    {
    return QIcon();
    }

  return QIcon(markupsNode->GetIcon());
}

//---------------------------------------------------------------------------
QIcon qSlicerSubjectHierarchyMarkupsPlugin::visibilityIcon(int visible)
{
  // Have the default plugin (which is not registered) take care of this
  return qSlicerSubjectHierarchyPluginHandler::instance()->defaultPlugin()->visibilityIcon(visible);
}

//-----------------------------------------------------------------------------
void qSlicerSubjectHierarchyMarkupsPlugin::setDisplayColor(vtkIdType itemID, QColor color, QMap<int, QVariant> terminologyMetaData)
{
  if (itemID == vtkMRMLSubjectHierarchyNode::INVALID_ITEM_ID)
    {
    qCritical() << Q_FUNC_INFO << ": Invalid input item";
    return;
    }
  vtkMRMLSubjectHierarchyNode* shNode = qSlicerSubjectHierarchyPluginHandler::instance()->subjectHierarchyNode();
  if (!shNode)
    {
    qCritical() << Q_FUNC_INFO << ": Failed to access subject hierarchy node";
    return;
    }

  // Get display node
  vtkMRMLDisplayableNode* displayableNode = vtkMRMLDisplayableNode::SafeDownCast(shNode->GetItemDataNode(itemID));
  if (!displayableNode)
    {
    qCritical() << Q_FUNC_INFO << ": Unable to find node for subject hierarchy item " << shNode->GetItemName(itemID).c_str();
    return;
    }
  vtkMRMLDisplayNode* displayNode = displayableNode->GetDisplayNode();
  if (!displayNode)
    {
    qCritical() << Q_FUNC_INFO << ": No display node";
    return;
    }

  // Set terminology metadata
  if (terminologyMetaData.contains(qSlicerTerminologyItemDelegate::TerminologyRole))
    {
    displayableNode->SetAttribute(vtkSegment::GetTerminologyEntryTagName(),
      terminologyMetaData[qSlicerTerminologyItemDelegate::TerminologyRole].toString().toUtf8().constData() );
    }
  if (terminologyMetaData.contains(qSlicerTerminologyItemDelegate::NameRole))
    {
    displayableNode->SetName(
      terminologyMetaData[qSlicerTerminologyItemDelegate::NameRole].toString().toUtf8().constData() );
    }
  if (terminologyMetaData.contains(qSlicerTerminologyItemDelegate::NameAutoGeneratedRole))
    {
    displayableNode->SetAttribute( vtkSlicerTerminologiesModuleLogic::GetNameAutoGeneratedAttributeName(),
      terminologyMetaData[qSlicerTerminologyItemDelegate::NameAutoGeneratedRole].toString().toUtf8().constData() );
    }
  if (terminologyMetaData.contains(qSlicerTerminologyItemDelegate::ColorAutoGeneratedRole))
    {
    displayableNode->SetAttribute( vtkSlicerTerminologiesModuleLogic::GetColorAutoGeneratedAttributeName(),
      terminologyMetaData[qSlicerTerminologyItemDelegate::ColorAutoGeneratedRole].toString().toUtf8().constData() );
    }

  // Set color
  double* oldColorArray = displayNode->GetColor();
  QColor oldColor = QColor::fromRgbF(oldColorArray[0], oldColorArray[1], oldColorArray[2]);
  if (oldColor != color)
    {
    displayNode->SetSelectedColor(color.redF(), color.greenF(), color.blueF());

    // Trigger update of color swatch
    shNode->ItemModified(itemID);
    }
}

//-----------------------------------------------------------------------------
QColor qSlicerSubjectHierarchyMarkupsPlugin::getDisplayColor(vtkIdType itemID, QMap<int, QVariant> &terminologyMetaData)const
{
  if (itemID == vtkMRMLSubjectHierarchyNode::INVALID_ITEM_ID)
    {
    qCritical() << Q_FUNC_INFO << ": Invalid input item";
    return QColor(0,0,0,0);
    }
  vtkMRMLSubjectHierarchyNode* shNode = qSlicerSubjectHierarchyPluginHandler::instance()->subjectHierarchyNode();
  if (!shNode)
    {
    qCritical() << Q_FUNC_INFO << ": Failed to access subject hierarchy node";
    return QColor(0,0,0,0);
    }
  vtkMRMLScene* scene = qSlicerSubjectHierarchyPluginHandler::instance()->mrmlScene();
  if (!scene)
    {
    qCritical() << Q_FUNC_INFO << ": Invalid MRML scene";
    return QColor(0,0,0,0);
    }

  if (scene->IsImporting())
    {
    // During import SH node may be created before the segmentation is read into the scene,
    // so don't attempt to access the segment yet
    return QColor(0,0,0,0);
    }

  // Get display node
  vtkMRMLDisplayableNode* displayableNode = vtkMRMLDisplayableNode::SafeDownCast(shNode->GetItemDataNode(itemID));
  if (!displayableNode)
    {
    qCritical() << Q_FUNC_INFO << ": Unable to find node for subject hierarchy item " << shNode->GetItemName(itemID).c_str();
    return QColor(0,0,0,0);
    }
  vtkMRMLDisplayNode* displayNode = displayableNode->GetDisplayNode();
  if (!displayNode)
    {
    // this is normal when the markups node is being created
    return QColor(0,0,0,0);
    }

  // Get terminology metadata
  terminologyMetaData.clear();
  terminologyMetaData[qSlicerTerminologyItemDelegate::TerminologyRole] =
    displayableNode->GetAttribute(vtkSegment::GetTerminologyEntryTagName());
  terminologyMetaData[qSlicerTerminologyItemDelegate::NameRole] = displayableNode->GetName();
  // If auto generated flags are not initialized, then set them to the default
  // (color: on, name: off - this way color will be set from the selector but name will not)
  bool nameAutoGenerated = false;
  if (displayableNode->GetAttribute(vtkSlicerTerminologiesModuleLogic::GetNameAutoGeneratedAttributeName()))
    {
    nameAutoGenerated = QVariant(displayableNode->GetAttribute(vtkSlicerTerminologiesModuleLogic::GetNameAutoGeneratedAttributeName())).toBool();
    }
  terminologyMetaData[qSlicerTerminologyItemDelegate::NameAutoGeneratedRole] = nameAutoGenerated;
  bool colorAutoGenerated = true;
  if (displayableNode->GetAttribute(vtkSlicerTerminologiesModuleLogic::GetColorAutoGeneratedAttributeName()))
    {
    colorAutoGenerated = QVariant(displayableNode->GetAttribute(vtkSlicerTerminologiesModuleLogic::GetColorAutoGeneratedAttributeName())).toBool();
    }
  terminologyMetaData[qSlicerTerminologyItemDelegate::ColorAutoGeneratedRole] = colorAutoGenerated;

  // Get and return color
  double* colorArray = displayNode->GetSelectedColor();
  return QColor::fromRgbF(colorArray[0], colorArray[1], colorArray[2]);
}

//-----------------------------------------------------------------------------
QList<QAction*> qSlicerSubjectHierarchyMarkupsPlugin::viewContextMenuActions()const
{
  Q_D(const qSlicerSubjectHierarchyMarkupsPlugin);

  QList<QAction*> actions;
  actions << d->RenamePointAction << d->DeletePointAction << d->ToggleSelectPointAction << d->ToggleHandleInteractive << d->HandleVisibilityAction;
  return actions;
}

//-----------------------------------------------------------------------------
void qSlicerSubjectHierarchyMarkupsPlugin::showViewContextMenuActionsForItem(vtkIdType itemID, QVariantMap eventData)
{
  Q_D(qSlicerSubjectHierarchyMarkupsPlugin);

  if (itemID == vtkMRMLSubjectHierarchyNode::INVALID_ITEM_ID)
    {
    qCritical() << Q_FUNC_INFO << ": Invalid input item";
    return;
    }
  vtkMRMLSubjectHierarchyNode* shNode = qSlicerSubjectHierarchyPluginHandler::instance()->subjectHierarchyNode();
  if (!shNode)
    {
    qCritical() << Q_FUNC_INFO << ": Failed to access subject hierarchy node";
    return;
    }

  // Markup
  vtkMRMLMarkupsNode* associatedNode = vtkMRMLMarkupsNode::SafeDownCast(shNode->GetItemDataNode(itemID));
  if (associatedNode)
    {
    d->ViewMenuEventData = eventData;
    d->ViewMenuEventData["NodeID"] = QVariant(associatedNode->GetID());

    int componentType = d->ViewMenuEventData["ComponentType"].toInt();
    bool pointActionsDisabled =
      componentType == vtkMRMLMarkupsDisplayNode::ComponentTranslationHandle ||
      componentType == vtkMRMLMarkupsDisplayNode::ComponentRotationHandle ||
      componentType == vtkMRMLMarkupsDisplayNode::ComponentScaleHandle ||
      componentType == vtkMRMLMarkupsDisplayNode::ComponentPlane ||
      componentType == vtkMRMLMarkupsROIDisplayNode::ComponentROI;

    d->RenamePointAction->setVisible(!pointActionsDisabled);
    d->DeletePointAction->setVisible(!pointActionsDisabled);
    d->ToggleSelectPointAction->setVisible(!pointActionsDisabled);

    vtkMRMLMarkupsDisplayNode* displayNode = vtkMRMLMarkupsDisplayNode::SafeDownCast(associatedNode->GetDisplayNode());
    d->ToggleHandleInteractive->setVisible(displayNode != nullptr);
    d->HandleVisibilityAction->setVisible(displayNode != nullptr);
    d->ToggleScaleHandleVisible->setVisible(vtkMRMLMarkupsROIDisplayNode::SafeDownCast(displayNode) != nullptr);
    if (displayNode)
      {
      d->ToggleHandleInteractive->setChecked(displayNode->GetHandlesInteractive());
      d->ToggleTranslateHandleVisible->setChecked(displayNode->GetTranslationHandleVisibility());
      d->ToggleRotateHandleVisible->setChecked(displayNode->GetRotationHandleVisibility());
      d->ToggleScaleHandleVisible->setChecked(displayNode->GetScaleHandleVisibility());
      }
    }
}

//-----------------------------------------------------------------------------
QList<QAction*> qSlicerSubjectHierarchyMarkupsPlugin::visibilityContextMenuActions() const
{
  Q_D(const qSlicerSubjectHierarchyMarkupsPlugin);

  QList<QAction*> actions;
  actions << d->ToggleCurrentItemHandleInteractive << d->CurrentItemHandleVisibilityAction;
  return actions;
}

//-----------------------------------------------------------------------------
void qSlicerSubjectHierarchyMarkupsPlugin::showVisibilityContextMenuActionsForItem(vtkIdType itemID)
{
  Q_D(qSlicerSubjectHierarchyMarkupsPlugin);

  if (itemID == vtkMRMLSubjectHierarchyNode::INVALID_ITEM_ID)
    {
    qCritical() << Q_FUNC_INFO << ": Invalid input item";
    return;
    }
  vtkMRMLSubjectHierarchyNode* shNode = qSlicerSubjectHierarchyPluginHandler::instance()->subjectHierarchyNode();
  if (!shNode)
    {
    qCritical() << Q_FUNC_INFO << ": Failed to access subject hierarchy node";
    return;
    }

  // Markup
  vtkMRMLMarkupsNode* associatedNode = vtkMRMLMarkupsNode::SafeDownCast(shNode->GetItemDataNode(itemID));
  if (associatedNode)
    {
    vtkMRMLMarkupsDisplayNode* displayNode = vtkMRMLMarkupsDisplayNode::SafeDownCast(associatedNode->GetDisplayNode());
    d->ToggleCurrentItemHandleInteractive->setVisible(displayNode != nullptr);
    d->CurrentItemHandleVisibilityAction->setVisible(displayNode != nullptr);
    d->ToggleCurrentItemScaleHandleVisible->setVisible(vtkMRMLMarkupsROIDisplayNode::SafeDownCast(displayNode) != nullptr);
    if (displayNode)
      {
      d->ToggleCurrentItemHandleInteractive->setChecked(displayNode->GetHandlesInteractive());
      d->ToggleCurrentItemTranslateHandleVisible->setChecked(displayNode->GetTranslationHandleVisibility());
      d->ToggleCurrentItemRotateHandleVisible->setChecked(displayNode->GetRotationHandleVisibility());
      d->ToggleCurrentItemScaleHandleVisible->setChecked(displayNode->GetScaleHandleVisibility());
      }
    }
}

//-----------------------------------------------------------------------------
void qSlicerSubjectHierarchyMarkupsPlugin::renamePoint()
{
  Q_D(qSlicerSubjectHierarchyMarkupsPlugin);

  if (d->ViewMenuEventData.find("NodeID") == d->ViewMenuEventData.end())
    {
    qCritical() << Q_FUNC_INFO << ": No node ID found in the view menu event data";
    return;
    }
  vtkMRMLScene* scene = qSlicerSubjectHierarchyPluginHandler::instance()->mrmlScene();
  if (!scene)
    {
    qCritical() << Q_FUNC_INFO << ": Failed to access MRML scene";
    return;
    }

  // Get markups node
  QString nodeID = d->ViewMenuEventData["NodeID"].toString();
  vtkMRMLNode* node = scene->GetNodeByID(nodeID.toUtf8().constData());
  vtkMRMLMarkupsNode* markupsNode = vtkMRMLMarkupsNode::SafeDownCast(node);
  if (!markupsNode)
    {
    qCritical() << Q_FUNC_INFO << ": Failed to get markups node by ID " << nodeID;
    return;
    }

  // Get point index
  int componentIndex = d->ViewMenuEventData["ComponentIndex"].toInt();

  // Pop up an entry box for the new name, with the old name as default
  QString oldName(markupsNode->GetNthControlPointLabel(componentIndex).c_str());

  bool ok = false;
  QString newName = QInputDialog::getText(nullptr, QString("Rename ") + oldName, "New name:", QLineEdit::Normal, oldName, &ok);
  if (!ok)
    {
    return;
    }

  markupsNode->SetNthControlPointLabel(componentIndex, newName.toUtf8().constData());
}

//-----------------------------------------------------------------------------
void qSlicerSubjectHierarchyMarkupsPlugin::deletePoint()
{
  Q_D(qSlicerSubjectHierarchyMarkupsPlugin);

  if (d->ViewMenuEventData.find("NodeID") == d->ViewMenuEventData.end())
    {
    qCritical() << Q_FUNC_INFO << ": No node ID found in the view menu event data";
    return;
    }
  vtkMRMLScene* scene = qSlicerSubjectHierarchyPluginHandler::instance()->mrmlScene();
  if (!scene)
    {
    qCritical() << Q_FUNC_INFO << ": Failed to access MRML scene";
    return;
    }

  // Get markups node
  QString nodeID = d->ViewMenuEventData["NodeID"].toString();
  vtkMRMLNode* node = scene->GetNodeByID(nodeID.toUtf8().constData());
  vtkMRMLMarkupsNode* markupsNode = vtkMRMLMarkupsNode::SafeDownCast(node);
  if (!markupsNode)
    {
    qCritical() << Q_FUNC_INFO << ": Failed to get markups node by ID " << nodeID;
    return;
    }

  // Get point index
  int componentIndex = d->ViewMenuEventData["ComponentIndex"].toInt();

  markupsNode->RemoveNthControlPoint(componentIndex);
}

//-----------------------------------------------------------------------------
void qSlicerSubjectHierarchyMarkupsPlugin::toggleSelectPoint()
{
  Q_D(qSlicerSubjectHierarchyMarkupsPlugin);

  if (d->ViewMenuEventData.find("NodeID") == d->ViewMenuEventData.end())
    {
    qCritical() << Q_FUNC_INFO << ": No node ID found in the view menu event data";
    return;
    }
  vtkMRMLScene* scene = qSlicerSubjectHierarchyPluginHandler::instance()->mrmlScene();
  if (!scene)
    {
    qCritical() << Q_FUNC_INFO << ": Failed to access MRML scene";
    return;
    }

  // Get markups node
  QString nodeID = d->ViewMenuEventData["NodeID"].toString();
  vtkMRMLNode* node = scene->GetNodeByID(nodeID.toUtf8().constData());
  vtkMRMLMarkupsNode* markupsNode = vtkMRMLMarkupsNode::SafeDownCast(node);
  if (!markupsNode)
    {
    qCritical() << Q_FUNC_INFO << ": Failed to get markups node by ID " << nodeID;
    return;
    }

  // Get point index
  int componentIndex = d->ViewMenuEventData["ComponentIndex"].toInt();

  markupsNode->SetNthControlPointSelected(componentIndex, !markupsNode->GetNthControlPointSelected(componentIndex));
}

//-----------------------------------------------------------------------------
void qSlicerSubjectHierarchyMarkupsPlugin::toggleHandleInteractive()
{
  Q_D(qSlicerSubjectHierarchyMarkupsPlugin);

  if (d->ViewMenuEventData.find("NodeID") == d->ViewMenuEventData.end())
    {
    qCritical() << Q_FUNC_INFO << ": No node ID found in the view menu event data";
    return;
    }
  vtkMRMLScene* scene = qSlicerSubjectHierarchyPluginHandler::instance()->mrmlScene();
  if (!scene)
    {
    qCritical() << Q_FUNC_INFO << ": Failed to access MRML scene";
    return;
    }

  // Get markups node
  QString nodeID = d->ViewMenuEventData["NodeID"].toString();
  vtkMRMLNode* node = scene->GetNodeByID(nodeID.toUtf8().constData());
  vtkMRMLMarkupsNode* markupsNode = vtkMRMLMarkupsNode::SafeDownCast(node);
  if (!markupsNode)
    {
    qCritical() << Q_FUNC_INFO << ": Failed to get markups node by ID " << nodeID;
    return;
    }

  vtkMRMLMarkupsDisplayNode* displayNode = vtkMRMLMarkupsDisplayNode::SafeDownCast(markupsNode->GetDisplayNode());
  if (!displayNode)
    {
    qCritical() << Q_FUNC_INFO << ": Failed to get display node for " << nodeID;
    return;
    }
  displayNode->SetHandlesInteractive(!displayNode->GetHandlesInteractive());
}

//-----------------------------------------------------------------------------
void qSlicerSubjectHierarchyMarkupsPlugin::toggleCurrentItemHandleInteractive()
{
  Q_D(qSlicerSubjectHierarchyMarkupsPlugin);

  // Get currently selected node and scene
  vtkMRMLSubjectHierarchyNode* shNode = qSlicerSubjectHierarchyPluginHandler::instance()->subjectHierarchyNode();
  if (!shNode)
    {
    qCritical() << Q_FUNC_INFO << ": Failed to access subject hierarchy node";
    return;
    }
  vtkIdType currentItemID = qSlicerSubjectHierarchyPluginHandler::instance()->currentItem();
  if (!currentItemID)
    {
    qCritical() << Q_FUNC_INFO << ": Invalid current subject hierarchy item";
    return;
    }

  vtkMRMLMarkupsNode* markupNode = vtkMRMLMarkupsNode::SafeDownCast(shNode->GetItemDataNode(currentItemID));
  if (!markupNode)
    {
    qCritical() << Q_FUNC_INFO << ": Invalid markup node";
    return;
    }

  markupNode->CreateDefaultDisplayNodes();
  vtkMRMLMarkupsDisplayNode* displayNode = vtkMRMLMarkupsDisplayNode::SafeDownCast(markupNode->GetDisplayNode());
  if (!displayNode)
    {
    qCritical() << Q_FUNC_INFO << ": Invalid display node";
    return;
    }

  displayNode->SetHandlesInteractive(!displayNode->GetHandlesInteractive());
}

//-----------------------------------------------------------------------------
void qSlicerSubjectHierarchyMarkupsPlugin::toggleCurrentItemHandleTypeVisibility()
{
  Q_D(qSlicerSubjectHierarchyMarkupsPlugin);

  if (d->ViewMenuEventData.find("NodeID") == d->ViewMenuEventData.end())
    {
    qCritical() << Q_FUNC_INFO << ": No node ID found in the view menu event data";
    return;
    }
  vtkMRMLScene* scene = qSlicerSubjectHierarchyPluginHandler::instance()->mrmlScene();
  if (!scene)
    {
    qCritical() << Q_FUNC_INFO << ": Failed to access MRML scene";
    return;
    }

  // Get markups node
  QString nodeID = d->ViewMenuEventData["NodeID"].toString();
  vtkMRMLNode* node = scene->GetNodeByID(nodeID.toUtf8().constData());
  vtkMRMLMarkupsNode* markupsNode = vtkMRMLMarkupsNode::SafeDownCast(node);
  if (!markupsNode)
    {
    qCritical() << Q_FUNC_INFO << ": Failed to get markups node by ID " << nodeID;
    return;
    }

  vtkMRMLMarkupsDisplayNode* displayNode = vtkMRMLMarkupsDisplayNode::SafeDownCast(markupsNode->GetDisplayNode());
  if (!displayNode)
    {
    qCritical() << Q_FUNC_INFO << ": Failed to get display node for " << nodeID;
    return;
    }

  QObject* sender = QObject::sender();
  if (!sender)
    {
    return;
    }

  int componentType = sender->property(INTERACTION_HANDLE_TYPE_PROPERTY).toInt();
  displayNode->SetHandleVisibility(componentType, !displayNode->GetHandleVisibility(componentType));
}

//-----------------------------------------------------------------------------
void qSlicerSubjectHierarchyMarkupsPlugin::toggleHandleTypeVisibility()
{
  Q_D(qSlicerSubjectHierarchyMarkupsPlugin);

  if (d->ViewMenuEventData.find("NodeID") == d->ViewMenuEventData.end())
    {
    qCritical() << Q_FUNC_INFO << ": No node ID found in the view menu event data";
    return;
    }
  vtkMRMLScene* scene = qSlicerSubjectHierarchyPluginHandler::instance()->mrmlScene();
  if (!scene)
    {
    qCritical() << Q_FUNC_INFO << ": Failed to access MRML scene";
    return;
    }

  // Get markups node
  QString nodeID = d->ViewMenuEventData["NodeID"].toString();
  vtkMRMLNode* node = scene->GetNodeByID(nodeID.toUtf8().constData());
  vtkMRMLMarkupsNode* markupsNode = vtkMRMLMarkupsNode::SafeDownCast(node);
  if (!markupsNode)
    {
    qCritical() << Q_FUNC_INFO << ": Failed to get markups node by ID " << nodeID;
    return;
    }

  vtkMRMLMarkupsDisplayNode* displayNode = vtkMRMLMarkupsDisplayNode::SafeDownCast(markupsNode->GetDisplayNode());
  if (!displayNode)
    {
    qCritical() << Q_FUNC_INFO << ": Failed to get display node for " << nodeID;
    return;
    }

  QObject* sender = QObject::sender();
  if (!sender)
    {
    return;
    }

  int componentType = sender->property(INTERACTION_HANDLE_TYPE_PROPERTY).toInt();
  displayNode->SetHandleVisibility(componentType, !displayNode->GetHandleVisibility(componentType));
}

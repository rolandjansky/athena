package atlantis.canvas;

import java.awt.Color;
import java.awt.Dimension;
import java.awt.Font;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.GraphicsEnvironment;
import java.awt.Insets;
import java.awt.Point;
import java.awt.Rectangle;
import java.awt.RenderingHints;
import java.awt.datatransfer.DataFlavor;
import java.awt.datatransfer.Transferable;
import java.awt.dnd.DnDConstants;
import java.awt.dnd.DragGestureEvent;
import java.awt.dnd.DragGestureListener;
import java.awt.dnd.DragSource;
import java.awt.dnd.DragSourceDragEvent;
import java.awt.dnd.DragSourceDropEvent;
import java.awt.dnd.DragSourceEvent;
import java.awt.dnd.DragSourceListener;
import java.awt.dnd.DropTarget;
import java.awt.dnd.DropTargetDragEvent;
import java.awt.dnd.DropTargetDropEvent;
import java.awt.dnd.DropTargetEvent;
import java.awt.dnd.DropTargetListener;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.geom.Point2D;
import java.awt.geom.Point2D.Double;
import java.awt.image.BufferedImage;
import java.awt.image.DataBuffer;

import java.util.Hashtable;
import java.util.LinkedList;
import java.util.Stack;
import java.util.Vector;

import javax.swing.JPanel;
import javax.swing.JPopupMenu;
import javax.swing.JSeparator;
import javax.swing.JToolTip;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;

import org.w3c.dom.NamedNodeMap;
import org.w3c.dom.Node;

import atlantis.globals.AGlobals;
import atlantis.graphics.ACoord;
import atlantis.graphics.dnd.ACallBack;
import atlantis.graphics.dnd.ADnDLabel;
import atlantis.graphics.dnd.AObjectTransferable;
import atlantis.gui.AColorHelpDialog;
import atlantis.gui.AInteractionToolBar;
import atlantis.gui.APreferencesControl;
import atlantis.interactions.AInteraction;
import atlantis.interactions.AInteractionsManager;
import atlantis.interactions.ASelection;
import atlantis.interactions.ARectangleVPSelection;
import atlantis.interactions.ARectangleYXSelection;
import atlantis.interactions.AZMRInteraction;
import atlantis.output.ALogInterface;
import atlantis.output.AOutput;
import atlantis.parameters.APar;
import atlantis.projection.AProjection;
import atlantis.projection.AProjection2D;
import atlantis.projection.AProjectionNPhysics;
import atlantis.projection.AProjectionTrackResidual;
import atlantis.projection.AProjectionsManager;
import atlantis.utils.AMath;

import atlantis.utils.ALogger;
import java.awt.BorderLayout;
import javax.swing.JComponent;

/**
 * The objects of this class are single windows managed by ACanvas
 * This class manages the painting of each picture and is always
 * called by the java awt event thread. Programatic requests for
 * picture updates are handled by calls to repaintFromScratch() which
 * adds requests to the awt event thread.
 * It paints first into an off-screen buffer which is then copied
 * onto the screen.
 * It handles the linear transformation from user to display
 * coordinates and vice-versa. Non-linear transformations are handled by
 * by the projections. Windows have a scale (and scale border) which
 * may be visible or not. Each window has an associated projection
 * and an interactionManager which knows about the current interaction
 * and the state of all other interactions for this window.
 *
 * @author    taylorg
 */
public class AWindow extends JPanel implements DropTargetListener,
       DragSourceListener, DragGestureListener, ACallBack
{   

  private static ALogger logger = ALogger.getLogger(AWindow.class);
  private static AGlobals globals = AGlobals.instance();
  
  //Whether horizontal or vertical scale is copied when window is dragged
  private static enum dragActionType { COPY_VERTICAL_SCALE, COPY_HORIZONTAL_SCALE };

  //these colors are used on the scale border and title bar
  static final Color BORDER_FOREGROUND_COLOR = new Color(48,48,48);
  static final Color BORDER_SELECTED_FOREGROUND_COLOR = new Color(48,48,48);
  static final Color BORDER_SELECTED_FRAME_COLOR = new Color(99,130,191);
  static final Color BORDER_SELECTED_FRAMELIGHT_COLOR = new Color(255,255,255);
  // These ones must be public and cannot be final because AOpenGLControl messes with them.
  public static Color BORDER_BACKGROUND_COLOR = new Color(224,224,224);
  public static Color BORDER_SELECTED_BACKGROUND_COLOR = new Color(200,221,242);
  private dragActionType dragAction;
  private boolean canCopyScales = false;
  private DragSource dragSource = null;
  private Graphics2D backingGraphics = null;
  private BufferedImage backingImage = null;
  private Dimension backingDimension = new Dimension();

  /**
   * The border of the window holds a scale with units
   **/
  private AScaleBorder scaleBorder;

  private boolean bufferValid = false;
  private boolean borderValid = false;
  private Insets insets = new Insets(0, 0, 0, 0);
  private double m00, m01, m02, m10, m11, m12, n00, n01, n02, n10, n11, n12;
  private AInteractionsManager interactionsManager;
  private Hashtable<String, Double[]> corners;
  private Point2D.Double[] userCorners;
  private Point2D.Double[] defaultCorners;
  private AProjection projection;
  private String group;
  private String defaultGroup;
  private String defaultProjectionName;
  private int index;
  boolean finalized;
  private Stack<Point2D.Double[]> cornersHistory = new Stack<Point2D.Double[]>();
  private AProjection projectionHistory = null;
  private Stack<Integer> zoomSummed = new Stack<Integer>();
  private Stack<Integer> zoomLayers = new Stack<Integer>();
  private Stack<Integer> zoomCalo = new Stack<Integer>();
  private Stack<Integer> zoomCaloLAr = new Stack<Integer>();
  private Stack<Integer> modeHistory = new Stack<Integer>();
  private Stack<Integer> cutHistoryLAr = new Stack<Integer>();
  private Stack<Integer> cutHistoryHEC = new Stack<Integer>();
  private Stack<Integer> layoutHistory = new Stack<Integer>();
  private boolean locked;
  private static JPopupMenu popupMenu;
  private final static String LOCK = "Freeze";
  private final static String UNLOCK = "Unfreeze";
  private final static String HORIZONTAL_FLIP = "Horizontal Flip";
  private final static String VERTICAL_FLIP = "Vertical Flip";
  private final static String ROTATE_BY_90_CLOCKWISE = "Rotate by 90";
  private final static String ROTATE_BY_90_ANTICLOCKWISE = "Rotate by -90";
  private final static String TOGGLE_SCALE = "Toggle Scale";
  private final static String RESTORE_DEFAULTS = "Restore Initial Size";

  private JComponent m_view = null;

  static
  {
    popupMenu = new JPopupMenu();
    ActionListener actionListener = new PopupMenuActionListener();
    popupMenu.add(HORIZONTAL_FLIP).addActionListener(actionListener);
    popupMenu.add(VERTICAL_FLIP).addActionListener(actionListener);
    popupMenu.add(new JSeparator());
    popupMenu.add(ROTATE_BY_90_CLOCKWISE).addActionListener(actionListener);
    popupMenu.add(ROTATE_BY_90_ANTICLOCKWISE).addActionListener(
        actionListener);
    popupMenu.add(new JSeparator());
    popupMenu.add(TOGGLE_SCALE).addActionListener(actionListener);
    popupMenu.add(RESTORE_DEFAULTS).addActionListener(actionListener);
    popupMenu.add(LOCK).addActionListener(actionListener);
    popupMenu.add(UNLOCK).addActionListener(actionListener);
  }


  private static class PopupMenuActionListener
      implements ActionListener
    {
      public void actionPerformed(ActionEvent e)
      {
        String action = e.getActionCommand();
        AWindow window = ACanvas.getCanvas().getCurrentWindow();
        if(action.equals(LOCK))
        {
          window.setLocked(true);
        }
        else if(action.equals(UNLOCK))
        {
          window.setLocked(false);
        }
        else if(action.equals(TOGGLE_SCALE))
        {
          window.toggleScale();
        }
        // temporary fix to avoid crashes
        if(window.getProjection() instanceof AProjection2D)
        {
          if(action.equals(RESTORE_DEFAULTS))
          {
            window.unzoomFull();
          }
          else if(action.equals(HORIZONTAL_FLIP))
          {
            AZMRInteraction.performFlip(AZMRInteraction.
                getMiddleVerticalLine(window),
                window);
          }
          else if(action.equals(VERTICAL_FLIP))
          {
            AZMRInteraction.performFlip(AZMRInteraction.
                getMiddleHorizontalLine(window),
                window);
          }
          else if(action.equals(ROTATE_BY_90_CLOCKWISE))
          {
            AZMRInteraction.performPlus90Rotation(window);
          }
          else if(action.equals(ROTATE_BY_90_ANTICLOCKWISE))
          {
            AZMRInteraction.performMinus90Rotation(window);
          }
        }
      }
    }

  /**
   * Construct AWindow using information in XML configuration file.
   * 
   * @param node the XML configuration node for this window
   * @param index identifier for the window
   * @return the constructed window
   */
  static AWindow createFromXMLConfigNode(Node node, int index) {
	  NamedNodeMap attributes = node.getAttributes();
	  String name = attributes.getNamedItem("name").getNodeValue();
	  String defaultProjectionName = attributes.getNamedItem("projection").
	    	      getNodeValue();
	  String defaultGroup = attributes.getNamedItem("group").getNodeValue();
	  return new AWindow(name,defaultProjectionName,defaultGroup,index);
  }
  
  /**
   * Construct AWindow with given name, default projection and group name.
   * Only non-private in order to allow test code to create an AWindow instance.
   * 
   * @param name title of window
   * @param defaultProjection name of default projection
   * @param defaultGroup name of group
   * @param index identifier for window
   */
  AWindow(String name, String defaultProjection, String defaultGroup, int index)
  {
    super(new BorderLayout());
    setName(name);
    this.defaultProjectionName = defaultProjection;
    this.defaultGroup = defaultGroup;
    setOpaque(false);
    if (!globals.getUseOpenGL()) {
      m_view = new AWindowSwingView(this);
    } else {
      m_view = new AWindowGLView(this);
    }
    this.index = index;
    scaleBorder = new AScaleBorder(this);
    scaleBorder.setLabelFont(new Font("Dialog", Font.PLAIN, 12));
    m_view.setBorder(scaleBorder);
    add(m_view, BorderLayout.CENTER);
    finalized = false;

    if (!AGlobals.isAtlantisHeadless()) {
      dragSource = new DragSource();
      dragSource.createDefaultDragGestureRecognizer(this,
          DnDConstants.ACTION_MOVE, this);

      this.setDropTarget(new DropTarget(this, this));
    }
  }

  /**
   * Perform the steps that have to follow the ACanvas initialization.
   * Despite its name, this method is called from ACanvas.setCurrentLayout()
   * so may be called multiple times on the same window.
   */
  void finalizeConstruction()
  {
	setInteractionManager(new AInteractionsManager(this));
	corners = new Hashtable<String, Double[]>();
    String[] pName = AProjectionsManager.getKnownProjectionNames();
    Point2D.Double[] pCorners;
    ALayout l = ACanvas.getCanvas().getCurrentLayout();
    Dimension size = l.getWindowConstraints(getName()).getSize();
    for(int i = 0; i < pName.length; i++)
    {
      AProjection proj = AProjectionsManager.getProjection(pName[i]);
      if(proj instanceof AProjection2D)
      {
        pCorners = ((AProjection2D) proj).calculateNoZoomCorners(size);
        corners.put(pName[i], pCorners);
      }
    }
    AProjection defaultProjection = AProjectionsManager.getProjection(
        defaultProjectionName);
    setGroup(defaultGroup);
    setProjection(defaultProjection);
    
   	if (!AGlobals.isAtlantisHeadless()) {
   		interactionsManager.setContext(getInteractionToolBar().getSelectedGroup());
    }

    finalized = true;
  }


  void restoreDefaults()
  {
    if(finalized)
    {
      corners = new Hashtable<String, Double[]>();
      String[] pName = AProjectionsManager.getKnownProjectionNames();
      Point2D.Double[] pCorners;
      ALayout l = ACanvas.getCanvas().getCurrentLayout();
      Rectangle r = l.getWindowConstraints(getName());
      Dimension size = new Dimension(1, 1);
      if(r != null)
        size = r.getSize();
      for(int i = 0; i < pName.length; i++)
      {
        AProjection proj = AProjectionsManager.getProjection(pName[i]);
        if(proj instanceof AProjection2D)
        {
          if(pName[i].equals(defaultProjectionName) && defaultCorners != null)
            pCorners = getDefaultCorners();
          else
            pCorners = ((AProjection2D) proj).calculateNoZoomCorners(size);
          corners.put(pName[i], pCorners);
        }
      }
      AProjection defaultProjection = AProjectionsManager.getProjection(
          defaultProjectionName);
      setGroup(defaultGroup);
      projection = null;
      setProjection(defaultProjection);
      getInteractionManager().forgetContext();
      AInteractionToolBar itb = getInteractionToolBar();
      if (itb != null) {
        itb.setDefaultGroup();
        interactionsManager.setContext(itb.getSelectedGroup());
      }
      setScaleStatus(true);
    }
  }


  public boolean isCurrent()
  {
    return ACanvas.getCanvas().getCurrentWindowName().equals(getName());
  }


  public static JPopupMenu getPopupMenu()
  {
    return popupMenu;
  }


  public int getIndex()
  {
    return index;
  }


  public String getGroupName()
  {
    return group;
  }


  public void setGroup(String group)
  {
    this.group = group;
    this.fireGroupChange();
  }


  public synchronized AProjection getProjection()
  {
    return projection;
  }


  public AInteractionToolBar getInteractionToolBar()
  {
    return AInteractionToolBar.getInteractionToolBar(getName(),
        projection.getName());
  }


  public synchronized void setProjection(AProjection newProjection)
  {
    if((projection == null) || (!newProjection.equals(projection))
        || (newProjection instanceof AProjectionTrackResidual)
        || (newProjection instanceof AProjectionNPhysics))
    {
      if(projection != null && projection instanceof AProjection2D)
        corners.put(projection.getName(), userCorners);
      this.projection = newProjection;
      if(projection instanceof AProjection2D)
        userCorners = (Point2D.Double[]) corners.get(projection.getName());
      //check to see if the scales need hiding or not    
      this.scaleBorder.hideScale();
      this.repaintFromScratch();
      this.fireProjectionChange();
    }
  }


  private Point2D.Double[] clonedCopy(Point2D.Double[] corners)
  {
    if(corners == null)return null;
    Point2D.Double[] newCorners = new Point2D.Double[corners.length];
    for(int i = 0; i < corners.length; i++)
      newCorners[i] = new Point2D.Double(corners[i].x, corners[i].y);
    return newCorners;
  }


  public synchronized void setUserCorners(Point2D.Double[] corners)
  {
    if(projection != null)
    {
      Point2D.Double[] validCorners = null;
      try
      {
        validCorners = ((AProjection2D) projection).validateCorners(
            clonedCopy(corners));
      }
      catch(ClassCastException e)
      {
        logger.error("Not a 2D projection...");
        validCorners = corners;
      }
      if(validCorners != null)
      {
        this.userCorners = validCorners;
        if(ACanvas.getCanvas().customisedCorner)
        {
          this.defaultCorners = validCorners;
        }
        repaintFromScratch();
      }
    }
  }


  public boolean getUnzoomPossible()
  {
    if(zoomSummed.size()>0 && ((Integer) zoomSummed.peek()).intValue()==cornersHistory.size())
      return false;
    else if(zoomLayers.size()>0 && ((Integer) zoomLayers.peek()).intValue()==cornersHistory.size())
      return false;
    else if(zoomCalo.size()>0 && ((Integer) zoomCalo.peek()).intValue()==cornersHistory.size())
      return false;
    else if(zoomCaloLAr.size()>0 && ((Integer) zoomCaloLAr.peek()).intValue()==cornersHistory.size())
      return false;
    else
      return projection == projectionHistory && cornersHistory.size() > 0;
  }

  public boolean getUnzoomFullPossible()
  {
    return projection == projectionHistory && cornersHistory.size() > 0;
  }


  public boolean getUnzoomAllPossible(String typeOfZoom)
  {
    if(cornersHistory.size()<=0)
      return false;
    else if(ARectangleVPSelection.ZOOM_LAR.equals(typeOfZoom) ||
        ARectangleVPSelection.ZOOM_CALORIMETERS.equals(typeOfZoom))
    {
      int indexOfZoomCalo=-1;
      if(zoomCalo.size()>0)
        indexOfZoomCalo=((Integer) zoomCalo.peek()).intValue();
      int indexOfZoomCaloLAr=-1;
      if(zoomCaloLAr.size()>0)
        indexOfZoomCaloLAr=((Integer) zoomCaloLAr.peek()).intValue();
      //make sure summed/layers is last in list
      if(ARectangleVPSelection.ZOOM_CALORIMETERS.equals(typeOfZoom) &&
          indexOfZoomCalo > indexOfZoomCaloLAr)
      {
        return true;
      }
      else if(typeOfZoom.equals(ARectangleVPSelection.ZOOM_LAR) &&
          indexOfZoomCaloLAr > indexOfZoomCalo)
      {
        return true;
      }
      else
        return false;
    }
    else
    {
      int indexOfZoomSummed=-1;
      if(zoomSummed.size()>0)
        indexOfZoomSummed=((Integer) zoomSummed.peek()).intValue();
      int indexOfZoomLayers=-1;
      if(zoomLayers.size()>0)
        indexOfZoomLayers=((Integer) zoomLayers.peek()).intValue();
      //make sure summed/layers is last in list
      if(typeOfZoom.equals(ARectangleYXSelection.ZOOM_SUMMED) && indexOfZoomSummed>indexOfZoomLayers)
      {
        return true;
      }
      else if(typeOfZoom.equals(ARectangleYXSelection.ZOOM_LAYERS) && indexOfZoomLayers>indexOfZoomSummed)
      {
        return true;
      }
      else
        return false;
    }
  }


  public void unzoom(String typeOfZoom)
  {
    //do unzoom of which item is selected
    int numberOfWindows=0,indexOfZoom;
    if(typeOfZoom.equals(ARectangleVPSelection.ZOOM_LAR) ||
        typeOfZoom.equals(ARectangleYXSelection.ZOOM_SUMMED))
    {
      numberOfWindows=4;
    }
    else if(ARectangleVPSelection.ZOOM_CALORIMETERS.equals(typeOfZoom) ||
        typeOfZoom.equals(ARectangleYXSelection.ZOOM_LAYERS))
    {
      numberOfWindows=8;
    }
    int numberOfUnlayouts=0;
    for (int i = 0; i < numberOfWindows; i++)
    {
      String wName = Integer.toString(i+1);
      AWindow w = ACanvas.getCanvas().getWindow(wName);
      //find if can do unzoom and what index it is
      indexOfZoom=findUnzoomIndex(typeOfZoom, w);
      if(indexOfZoom>0)
      {
        //do all unzooms upto and including index of this zoom to unzoom
        while(w.cornersHistory.size()>=indexOfZoom)
          w.unzoom();
        //also need to reset parameters
        setParameters(typeOfZoom, w, wName);
        //find out how many unlayouts to do
        int count=0;
        while(w.layoutHistory.size()>0 && ((Integer)w.layoutHistory.peek()).intValue()>=indexOfZoom)
        {
          w.layoutHistory.pop();
          count++;
        }
        if(count>numberOfUnlayouts)
          numberOfUnlayouts=count;
      }
    }
    //do as many unlayouts as needed
    for(int i=0; i<numberOfUnlayouts; i++)
      ACanvas.getCanvas().unlayout();
  }


  private int findUnzoomIndex(String typeOfZoom, AWindow w)
  {
    int indexOfZoom=0;//if returns 0 then no unzoom is to be performed
    if(ARectangleVPSelection.ZOOM_CALORIMETERS.equals(typeOfZoom))
    {
      if(w.zoomCalo.size()==0)
        indexOfZoom=0;
      else
      {
        indexOfZoom=((Integer) w.zoomCalo.pop()).intValue();
        //check if other zooms done on this window
        while(w.zoomCaloLAr.size()>0 && ((Integer) w.zoomCaloLAr.peek()).intValue()>indexOfZoom)
        {
          w.zoomCaloLAr.pop();
          w.modeHistory.pop();
        }
      }
    }
    else if(typeOfZoom.equals(ARectangleVPSelection.ZOOM_LAR))
    {
      if(w.zoomCaloLAr.size()==0)
        indexOfZoom=0;
      else
      {
        indexOfZoom=((Integer) w.zoomCaloLAr.pop()).intValue();
        while(w.zoomCalo.size()>0 && ((Integer) w.zoomCalo.peek()).intValue()>indexOfZoom)
        {
          w.zoomCalo.pop();
          w.modeHistory.pop();
        }
      }
    }
    if(typeOfZoom.equals(ARectangleYXSelection.ZOOM_SUMMED))
    {
      if(w.zoomSummed.size()==0)
        indexOfZoom=0;
      else
      {
        indexOfZoom=((Integer) w.zoomSummed.pop()).intValue();
        //check if other zooms done on this window
        while(w.zoomLayers.size()>0 && ((Integer) w.zoomLayers.peek()).intValue()>indexOfZoom)
        {
          w.zoomLayers.pop();
          w.modeHistory.pop();
          w.cutHistoryHEC.pop();
          w.cutHistoryLAr.pop();
        }
      }
    }
    else if(typeOfZoom.equals(ARectangleYXSelection.ZOOM_LAYERS))
    {
      if(w.zoomLayers.size()==0)
        indexOfZoom=0;
      else
      {
        indexOfZoom=((Integer) w.zoomLayers.pop()).intValue();
        while(w.zoomSummed.size()>0 && ((Integer) w.zoomSummed.peek()).intValue()>indexOfZoom)
        {
          w.zoomSummed.pop();
          w.modeHistory.pop();
          w.cutHistoryHEC.pop();
          w.cutHistoryLAr.pop();
        }
      }
    }
    return indexOfZoom;
  }


  private void setParameters(String typeOfZoom, AWindow w, String wName)
  {
    APar parameterStore = APar.instance();
    parameterStore.selectWindowParameters(wName);
    if(ARectangleVPSelection.ZOOM_LAR.equals(typeOfZoom) ||
        ARectangleVPSelection.ZOOM_CALORIMETERS.equals(typeOfZoom))
    {
      parameterStore.get("VP", "Mode").setI( w.modeHistory.pop() );
    }
    else
    {
      parameterStore.get("YX", "Mode").setI( w.modeHistory.pop() );
      parameterStore.get("CutsCalo","HEC").setI( w.cutHistoryHEC.pop() );
      parameterStore.get("CutsCalo","LAr").setI( w.cutHistoryLAr.pop() );
    }
    parameterStore.restoreWindowParameters();
  }


  public void saveParameters(String typeOfZoom)
  {
    APar parameterStore = APar.instance();
    //save window parameters of current windows
    if(typeOfZoom.equals(ARectangleYXSelection.ZOOM_SUMMED) || typeOfZoom.equals(ARectangleYXSelection.ZOOM_LAYERS))
    {
      parameterStore.selectWindowParameters(this.getName());
      this.modeHistory.push(new Integer(parameterStore.get("YX", "Mode").getI()));
      this.cutHistoryHEC.push(new Integer(parameterStore.get("CutsCalo","HEC").getI()));
      this.cutHistoryLAr.push(new Integer(parameterStore.get("CutsCalo","LAr").getI()));
      parameterStore.restoreWindowParameters();
    }
    else if(ARectangleVPSelection.ZOOM_CALORIMETERS.equals(typeOfZoom) ||
        ARectangleVPSelection.ZOOM_LAR.equals(typeOfZoom))
    {
      parameterStore.selectWindowParameters(this.getName());
      this.modeHistory.push(new Integer(parameterStore.get("VP", "Mode").getI()));
      parameterStore.restoreWindowParameters();
    }
  }


  public void unzoomFull()
  {
    //perform unzoom to initial state
    setUserCorners(((AProjection2D) getProjection()).calculateNoZoomCorners(getSize()));
    //clear all variables for this window
    cornersHistory.clear();
    zoomSummed.clear();
    zoomLayers.clear();
    zoomCaloLAr.clear();
    zoomCalo.clear();
    layoutHistory.clear();
    modeHistory.clear();
    cutHistoryHEC.clear();
    cutHistoryLAr.clear();
  }


  public void unzoom()
  {
    if(projection == projectionHistory && cornersHistory.size() > 0)
    {
      setUserCorners((Point2D.Double[]) (cornersHistory.pop()));
    }
  }


  public void saveLayout()
  {
    if(userCorners != null)
    {
      ACanvas.getCanvas().saveLayout();
    }
  }


  public void saveCorners()
  {
    if(projection != projectionHistory)
    {
      cornersHistory.clear();
      String wName = this.getName();
      int temp;
      if(wName.equals("1") || wName.equals("2") || wName.equals("3") || wName.equals("4") ||
          wName.equals("5") || wName.equals("6") || wName.equals("7") || wName.equals("8"))
      {
        zoomSummed.clear();
        zoomLayers.clear();
        zoomCaloLAr.clear();
        zoomCalo.clear();
        layoutHistory.clear();
        //will have already done saveParameters so only
        //want the last saved parameters of mode and cuts
        if(modeHistory.size()>0)
        {
          temp = ((Integer) modeHistory.pop()).intValue();
          modeHistory.clear();
          modeHistory.push(new Integer(temp));
        }

        if(cutHistoryHEC.size()>0)
        {
          temp = ((Integer) cutHistoryHEC.pop()).intValue();
          cutHistoryHEC.clear();
          cutHistoryHEC.push(new Integer(temp));
        }

        if(cutHistoryLAr.size()>0)
        {
          temp = ((Integer) cutHistoryLAr.pop()).intValue();
          cutHistoryLAr.clear();
          cutHistoryLAr.push(new Integer(temp));
        }
      }
    }
    projectionHistory = projection;
    if(userCorners != null)
    {
      cornersHistory.push(clonedCopy(userCorners));
    }
  }


  public void saveCorners(String typeOfZoom)
  {
    saveCorners();
    //save zoom just done
    if(userCorners != null)
    {
      if(typeOfZoom.equals(ARectangleYXSelection.ZOOM_SUMMED))
      {
        zoomSummed.push(new Integer(cornersHistory.size()));
      }
      else if(typeOfZoom.equals(ARectangleYXSelection.ZOOM_LAYERS))
      {
        zoomLayers.push(new Integer(cornersHistory.size()));
      }
      else if(ARectangleVPSelection.ZOOM_CALORIMETERS.equals(typeOfZoom))
      {
        zoomCalo.push(new Integer(cornersHistory.size()));
      }
      else if(ARectangleVPSelection.ZOOM_LAR.equals(typeOfZoom))
      {
        zoomCaloLAr.push(new Integer(cornersHistory.size()));
      }
      layoutHistory.push(new Integer(cornersHistory.size()));
    }
  }


  public void setUserCornersNoRepaint(Point2D.Double[] corners)
  {
    Point2D.Double[] validCorners = ((AProjection2D) projection).
      validateCorners(clonedCopy(corners));
    if(validCorners != null)
    {
      this.userCorners = validCorners;
    }
  }


  public void setUserCorners(double x0, double x1, double y0, double y1)
  {
    Point2D.Double[] corners = new Point2D.Double[3];
    corners[0] = new Point2D.Double(x0, y1);
    corners[1] = new Point2D.Double(x1, y1);
    corners[2] = new Point2D.Double(x1, y0);
    if(projection instanceof AProjection2D) corners = ((AProjection2D)
        projection).validateCorners(corners);
    if(corners != null)
      this.userCorners = corners;
  }


  public Point2D.Double[] getDefaultCorners()
  {
    return clonedCopy(defaultCorners);
  }


  public Point2D.Double[] getUserCorners()
  {
    return clonedCopy(userCorners);
  }


  public void setInteractionManager(AInteractionsManager interactionsManager)
  {
    this.interactionsManager = interactionsManager;
    //Make the interactions Manager aware of canvas changes
    ACanvas.getCanvas().addParentWindowFocusListener(interactionsManager);
  }


  public void setScaleStatus(boolean statusWanted)
  {
    if(getScaleStatus() != statusWanted) toggleScale();
  }


  public boolean getScaleStatus()
  {
    return scaleBorder.getScaleStatus();
  }


  private void toggleScale()
  {	
    scaleBorder.toggleScale();
    repaintFromScratch();
  }


  public AInteractionsManager getInteractionManager()
  {
    return interactionsManager;
  }


  public void paintComponent(Graphics g) {
    // Don't draw anything for ourself
  }

  void paintWindow(Graphics g, boolean doubleBuffer)
  {
    if(!ACanvas.getCanvas().isReallyOnScreen(this))
      return;
    //Why is this checked in paintComponent, and not already in 
    //setProjection() ? FIXIT / CLEANUP S.B
    if(projection instanceof AProjectionTrackResidual)
    {
      if (((AProjectionTrackResidual) projection).getResidual() != null)
        userCorners = ((AProjectionTrackResidual)projection).calculateNoZoomCorners();
      else
      {
        AOutput.append("\nTrackResidual Projection is not available, using YX instead in window "
            + this.getName() + ".\n", ALogInterface.WARNING);
        projection = AProjectionsManager.getProjection("YX");
        userCorners = (Point2D.Double[]) corners.get(projection.getName());
      }
    }

    //Make sure the offscreen buffer is there if we need one
    Graphics paint = g;
    if (doubleBuffer) {
      createOffScreenBuffer();
      paint = backingGraphics;
    }

    // Means either we're using g or backingGraphics was something valid
    if (paint != null)
    {
      if(!bufferValid)
      {
        setLocked(false);
        paintWindowFromScratch(paint);
        LinkedList<AInteraction> interactions = interactionsManager.getInteractions();
        for (int i = 0; i < interactions.size(); i++) {
          if (interactions.get(i) instanceof ASelection) {
            ((ASelection) interactions.get(i)).invalidate();
          }
        }
        bufferValid = true;

      }

      if (!isLocked() && !borderValid) {
        //m_view.getBorder().paintBorder(paint);
        borderValid = true;
      }

    }

    // If we painted into a context which wasn't the target graphics context
    // then we need to paint our buffer to the screen
    if (g != paint) {
      imageComponent(g);
    }

    LinkedList<AInteraction> interactions = interactionsManager.getInteractions();
    for(int i = 0; i < interactions.size(); i++)
      ((AInteraction) interactions.get(i)).paint((Graphics2D) g);
  }

  // I suppose this function is intended to be called before every printing,
  // but it is actually only called for EPS printing!!! FIX / CLEANUP S.B.
  public void printComponent(Graphics g)
  {
    setLocked(false);
    if(!ACanvas.getCanvas().isReallyOnScreen(this))
      return;
    ((AScaleBorder) getView().getBorder()).forceDraw();
    //Double border painting needed here for EPS - probably something wrong in
    //projection drawing -> graphics context left unclean? FIXIT. S.B.
    ((AScaleBorder) getView().getBorder()).printBorder(this,g,this.getX(),this.getY(),this.getWidth(),this.getHeight());
    paintWindowFromScratch(g);
    ((AScaleBorder) getView().getBorder()).printBorder(this,g,this.getX(),this.getY(),this.getWidth(),this.getHeight());
    borderValid = true;
    LinkedList<AInteraction> interactions = interactionsManager.getInteractions();
    for(int i = 0; i < interactions.size(); i++)
    {
      AInteraction inter = (AInteraction) interactions.get(i);
      if(inter.isPrintable())
        inter.paint((Graphics2D) g);
    }
  }


    public void printBorder(Graphics g){}


    // must call paint border myself since the border is backed too
    public void paintBorder(Graphics g)
    {}


    public void repaintFromScratch()
    {
        if(locked)return;
        bufferValid = false;
        borderValid = false;
        
        // Check whether to draw scales on repaint
        scaleBorder.hideScale();
        
        // For some reason need to repaint the view too when using heavyweight
        // component inside... - Adam
        repaint();
        m_view.repaint();
        
    }


    // Attention: for efficiency this routine overwrites and returns its input
    public ACoord calculateUser(ACoord user)
    {
        double[] h;
        double[] v;
        double hTemp;
        double vTemp;
        if(validTransform())
        {
            for(int j = 0; j < user.hv[0].length; ++j)
            {
                h = user.hv[0][j];
                v = user.hv[1][j];
                for(int i = 0; i < h.length; ++i)
                {
                    hTemp = h[i];
                    vTemp = v[i];
                    h[i] = m00 * hTemp + m01 * vTemp + m02;
                    v[i] = m10 * hTemp + m11 * vTemp + m12;
                }
            }
        }
        else
        {
            for(int j = 0; j < user.hv[0].length; ++j)
            {
                h = user.hv[0][j];
                v = user.hv[1][j];
                for(int i = 0; i < h.length; ++i)
                {
                    h[i] = 0.;
                    v[i] = 0.;
                }
            }
        }
        return user;
    }


    public Point2D.Double calculateUser(double x, double y)
    {
        double[][][] hv = new double[2][1][1];
        hv[0][0][0] = x;
        hv[1][0][0] = y;
        ACoord p = calculateUser(new ACoord(hv));
        return new Point2D.Double(p.hv[0][0][0], p.hv[1][0][0]);
    }


    public Point2D.Double calculateUser(Point2D.Double p)
    {
        return calculateUser(p.x, p.y);
    }


    public Point2D.Double calculateUser(Point p)
    {
        return calculateUser(p.x, p.y);
    }


    // Attention! For efficiency this routine overwrites and returns its input
    public ACoord calculateDisplay(ACoord user)
    {
        if(user == null)
            return null;
        double[] h;
        double[] v;
        double hTemp;
        double vTemp;
        if(validTransform())
        {
            for(int j = 0; j < user.hv[0].length; ++j)
            {
                h = user.hv[0][j];
                v = user.hv[1][j];
                for(int i = 0; i < h.length; ++i)
                {
                    hTemp = h[i];
                    vTemp = v[i];
                    h[i] = n00 * hTemp + n01 * vTemp + n02;
                    v[i] = n10 * hTemp + n11 * vTemp + n12;
                }
            }
        }
        else
        {
            for(int j = 0; j < user.hv[0].length; ++j)
            {
                h = user.hv[0][j];
                v = user.hv[1][j];
                for(int i = 0; i < h.length; ++i)
                {
                    h[i] = 0.;
                    v[i] = 0.;
                }
            }
        }
        return user;
    }


    public Point2D.Double calculateDisplay(double x, double y)
    {
        double[][][] hv = new double[2][1][1];
        hv[0][0][0] = x;
        hv[1][0][0] = y;
        ACoord p = calculateDisplay(new ACoord(hv));
        return new Point2D.Double(p.hv[0][0][0], p.hv[1][0][0]);
    }


    public Point2D.Double calculateDisplay(Point2D.Double p)
    {
        return calculateDisplay(p.x, p.y);
    }

    /** 
     * Called when the window is selected
     */
    void select()
    {
        scaleBorder.setSelected();
        //redraw border to indicate this window is selected
        borderValid = false;
        repaint();
    }

    /** 
     * Called when the window is deselected
     */
    void deselect()
    {
        scaleBorder.setDeselected();
        //redraw border to indicate this window is deselected
        borderValid = false;
        repaint();
    }

    /** 
     * Draws the image from the backup buffer on screen
     */
    private void imageComponent(Graphics g)
    {
        Graphics2D g2d = (Graphics2D) g;
        if(g2d != null)
        {
            Rectangle clipRect = g2d.getClipBounds();
            // Repaint only what is necessary. Except when running on MacOSX Tiger.
            // (see http://lists.apple.com/archives/Java-dev/2005/Jun/msg00611.html)
            if(clipRect != null && !(System.getProperty("os.name").equals("Mac OS X")
                && System.getProperty("os.version").startsWith("10.4.")))
            {
                int x = clipRect.x;
                int y = clipRect.y;
                int w = clipRect.width;
                int h = clipRect.height;
                BufferedImage subImage = backingImage.getSubimage(x, y, w, h);
                g2d.drawImage(subImage, x, y, this);
            }
            else
            {
                g2d.drawImage(backingImage, 0, 0, this);
            }
        }
    }


    public Rectangle getCurrDisp()
    {
        int width = m_view.getWidth();
        int height = m_view.getHeight();
        insets = m_view.getInsets(insets);
        int x = insets.left;
        int y = insets.top;
        int w = width - insets.left - insets.right;
        int h = height - insets.top - insets.bottom;
        return new Rectangle(x, y, w, h);
    }


    public void paintWindowFromScratch(Graphics g)
    {
        APar parameterStore = APar.instance();
        ACanvas.getCanvas().setPaintingWindow(this);
        parameterStore.selectWindowParameters(getName());
        Rectangle currDisp = getCurrDisp();
        int x = (int) Math.rint(currDisp.getX());
        int y = (int) Math.rint(currDisp.getY());
        int width = (int) Math.rint(currDisp.getWidth());
        int height = (int) Math.rint(currDisp.getHeight());
        g.setClip(x, y, width, height);
        Graphics2D g2 = (Graphics2D)g;
        if(APreferencesControl.getAliasMenuItem())
            g2.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);
        else
            g2.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_OFF);
        
        projection.paint(this, g);

        g.setClip(0, 0, m_view.getWidth(), m_view.getHeight());
        parameterStore.restoreWindowParameters();
        ACanvas.getCanvas().setPaintingWindow(null);
        
        if(AColorHelpDialog.getInstance() != null && AColorHelpDialog.getInstance().isVisible())
            AColorHelpDialog.getInstance().update();
    }


    private void createOffScreenBuffer()
    {
        int w = m_view.getWidth();
        int h = m_view.getHeight();
        Dimension dim = new Dimension(w, h);
        // Check that the current size is positive.
        if(w > 0 && h > 0)
        {
            if(!backingDimension.equals(dim))
            {
                // Usinig ABufferedPixelGraphics is not a good idea in MacOSX Tiger.
                // So instead we use createGraphics() to create a normal Graphics2D
                // object to draw on.
                if(System.getProperty("os.name").equals("Mac OS X")
                    && System.getProperty("os.version").startsWith("10.4."))
                {
                    backingImage = getGraphicsConfiguration().createCompatibleImage(w, h);
                } else if(GraphicsEnvironment.getLocalGraphicsEnvironment().
                   getDefaultScreenDevice().getDefaultConfiguration().
                   getColorModel().getTransferType()
                   == DataBuffer.TYPE_USHORT)
                {
                    backingImage = new BufferedImage(w, h,
                        BufferedImage.TYPE_USHORT_565_RGB);
                }
                else
                {
                    if(System.getProperty("os.name").equals("Mac OS X"))
                        backingImage = new BufferedImage(w, h,
                            BufferedImage.TYPE_INT_ARGB_PRE);
                    else
                        backingImage = new BufferedImage(w, h,
                            BufferedImage.TYPE_INT_RGB);
                }
                backingGraphics = backingImage.createGraphics();
                backingDimension.setSize(dim);
                bufferValid = false;
                borderValid = false;
            }
        }
        else
        {
            backingImage = null;
            backingGraphics = null;
        }
    }


    public Point2D.Double[] getDisplayCorners()
    {
        Point2D.Double[] corners = new Point2D.Double[3];
        insets = m_view.getInsets(insets);
        int x = insets.left;
        int y = insets.top;
        int w = m_view.getWidth() - insets.left - insets.right;
        int h = m_view.getHeight() - insets.top - insets.bottom;
        Point2D.Double upperLeft = new Point2D.Double(x, y);
        Point2D.Double lowerRight = new Point2D.Double(x + w, y + h);
        Point2D.Double upperRight = new Point2D.Double(x + w, y);
        corners[0] = upperLeft;
        corners[1] = upperRight;
        corners[2] = lowerRight;
        return corners;
    }


    private boolean validTransform()
    {
        double x1, y1, x2, y2, x3, y3, u1, v1, u2, v2, u3, v3;
        x1 = userCorners[0].x;
        y1 = userCorners[0].y;
        x2 = userCorners[1].x;
        y2 = userCorners[1].y;
        x3 = userCorners[2].x;
        y3 = userCorners[2].y;
        Point2D.Double[] displayCorners = getDisplayCorners();
        u1 = displayCorners[0].x;
        v1 = displayCorners[0].y;
        u2 = displayCorners[1].x;
        v2 = displayCorners[1].y;
        u3 = displayCorners[2].x;
        v3 = displayCorners[2].y;
        if(v1 * (u3 - u2) + v2 * (u1 - u3) + v3 * (u2 - u1) == 0.)return false;
        if((u1 - u2) == 0.)return false;
        m01 = (u1 * (x2 - x3) + u2 * (x3 - x1) + u3 * (x1 - x2)) /
              (v1 * (u3 - u2) + v2 * (u1 - u3) + v3 * (u2 - u1));
        m00 = ((x1 - x2) - m01 * (v1 - v2)) / (u1 - u2);
        m02 = x1 - m00 * u1 - m01 * v1;
        m11 = (u1 * (y2 - y3) + u2 * (y3 - y1) + u3 * (y1 - y2)) /
              (v1 * (u3 - u2) + v2 * (u1 - u3) + v3 * (u2 - u1));
        m10 = ((y1 - y2) - m11 * (v1 - v2)) / (u1 - u2);
        m12 = y1 - m10 * u1 - m11 * v1;
        if((x1 - x2) == 0.)
        {
            n01 = (u2 - u1) / (y2 - y1);
            n11 = (v2 - v1) / (y2 - y1);
            n00 = ((u3 - u2) - n01 * (y3 - y2)) / (x3 - x1);
            n10 = ((v3 - v2) - n11 * (y3 - y2)) / (x3 - x1);
            n02 = u1 - n00 * x1 - n01 * y1;
            n12 = v1 - n10 * x1 - n11 * y1;
            return true;
        }
        else
        {
            if(y1 * (x3 - x2) + y2 * (x1 - x3) + y3 * (x2 - x1) == 0.)return false;
            n01 = (x1 * (u2 - u3) + x2 * (u3 - u1) + x3 * (u1 - u2)) /
                  (y1 * (x3 - x2) + y2 * (x1 - x3) + y3 * (x2 - x1));
            n00 = ((u1 - u2) - n01 * (y1 - y2)) / (x1 - x2);
            n02 = u1 - n00 * x1 - n01 * y1;
            n11 = (x1 * (v2 - v3) + x2 * (v3 - v1) + x3 * (v1 - v2)) /
                  (y1 * (x3 - x2) + y2 * (x1 - x3) + y3 * (x2 - x1));
            n10 = ((v1 - v2) - n11 * (y1 - y2)) / (x1 - x2);
            n12 = v1 - n10 * x1 - n11 * y1;
            return true;
        }
    }


    public JToolTip createToolTip()
    {
        JToolTip tip = new JToolTip();
        tip.setFont(new Font(null, Font.PLAIN, 20));
        return tip;
    }


    // implementation of DropTargetListener

    // Called when a drag operation has encountered the DropTarget.
    public void dragEnter(DropTargetDragEvent dtde)
    {
        dtde.acceptDrag(DnDConstants.ACTION_MOVE);
    }


    // The drag operation has departed the DropTarget without dropping.
    public void dragExit(DropTargetEvent dte)
    {}


    // Called when a drag operation is ongoing on the DropTarget.
    public void dragOver(DropTargetDragEvent dtde)
    {}


    // The drag operation has terminated with a drop on this DropTarget.
    public void drop(DropTargetDropEvent dtde)
    {
        try
        {
            Transferable transferable = dtde.getTransferable();
            DataFlavor javaObjectDataFlavor = new DataFlavor(DataFlavor.
                javaJVMLocalObjectMimeType);
            if(transferable.isDataFlavorSupported(javaObjectDataFlavor))
            {
                dtde.acceptDrop(DnDConstants.ACTION_MOVE);
                Object object = transferable.getTransferData(
                    javaObjectDataFlavor);
                if(object instanceof ACallBack)
                    ((ACallBack) object).callBack(this);
                else
                    ACanvas.getCanvas().copyWindowSettings(((ADnDLabel) object).getText(),
                                             getName());
                dtde.getDropTargetContext().dropComplete(true);
            }
            else
                AOutput.append("Cannot accept drop\n", ALogInterface.BAD_COMMAND);
        }
        catch(Exception e)
        {
            e.printStackTrace();
            dtde.rejectDrop();
        }
    }


    // Called if the user has modified the current drop gesture.
    public void dropActionChanged(DropTargetDragEvent dtde)
    {}


    private Vector<ChangeListener> projectionChangeListeners = new Vector<ChangeListener>();
    public void addProjectionChangeListener(ChangeListener listener)
    {
        projectionChangeListeners.addElement(listener);
        listener.stateChanged(new ChangeEvent(this));
    }


    private void fireProjectionChange()
    {
        ChangeEvent changeEvent = new ChangeEvent(this);
        for(int i = 0; i < projectionChangeListeners.size(); i++)
        {
            ChangeListener changeListener = (ChangeListener)
                                            projectionChangeListeners.elementAt(
                i);
            changeListener.stateChanged(changeEvent);
        }
    }


    private Vector<ChangeListener> groupChangeListeners = new Vector<ChangeListener>();
    public void addGroupChangeListener(ChangeListener listener)
    {
        groupChangeListeners.addElement(listener);
        listener.stateChanged(new ChangeEvent(this));
    }


    private void fireGroupChange()
    {
        ChangeEvent changeEvent = new ChangeEvent(this);
        for(int i = 0; i < groupChangeListeners.size(); i++)
        {
            ChangeListener changeListener = (ChangeListener)
                                            groupChangeListeners.elementAt(i);
            changeListener.stateChanged(changeEvent);
        }
    }


    // implementation of DragSourceListener

    // This method is invoked to signify that the Drag and Drop operation is complete.
    public void dragDropEnd(DragSourceDropEvent dsde)
    {}


    // Called as the hotspot enters a platform dependent drop site.
    public void dragEnter(DragSourceDragEvent dsde)
    {
        dsde.getDragSourceContext().setCursor(ADnDLabel.DROP_INVALID);
    }


    // Called as the hotspot exits a platform dependent drop site.
    public void dragExit(DragSourceEvent dse)
    {
        dse.getDragSourceContext().setCursor(ADnDLabel.DROP_INVALID);
    }


    // Called as the hotspot moves over a platform dependent drop site.
    public void dragOver(DragSourceDragEvent dsde)
    {
        dsde.getDragSourceContext().setCursor(ADnDLabel.DROP_VALID);
    }


    // Called when the user has modified the drop gesture.
    public void dropActionChanged(DragSourceDragEvent dsde)
    {}


    // implementation of DragGestureListener

    public void dragGestureRecognized(DragGestureEvent dge)
    {
        if(!canCopyScales)return;
        Point p = dge.getDragOrigin();
        Insets i = m_view.getInsets();
        int w = m_view.getWidth();
        int h = m_view.getHeight();
        Rectangle vScale = new Rectangle(0, 0, i.left, h - i.bottom);
        Rectangle hScale = new Rectangle(i.left, h - i.bottom, w - i.left,
                                         i.bottom);
        if(vScale.contains(p))
            dragAction = dragActionType.COPY_VERTICAL_SCALE;
        else if(hScale.contains(p))
            dragAction = dragActionType.COPY_HORIZONTAL_SCALE;
        else
            return;
        dragSource.startDrag(dge, ADnDLabel.DROP_INVALID,
                             new AObjectTransferable(this), this);
    }


    public void setScaleCopyStatus(boolean canCopyScales)
    {
        this.canCopyScales = canCopyScales;
    }


    public void callBack(Object to)
    {
        AWindow targetWindow;
        if(to instanceof ADnDLabel)
            targetWindow = ACanvas.getCanvas().getWindow(((ADnDLabel) to).
                getText());
        else if(to instanceof AWindow)
            targetWindow = (AWindow) to;
        else
            return;
        if(dragAction == dragActionType.COPY_VERTICAL_SCALE)
        {
            String labelSource = ((AScaleBorder) getBorder()).
                                 getVerticalAxisLabel();
            String labelTarget = ((AScaleBorder) (targetWindow.getBorder())).
                                 getVerticalAxisLabel();
            if(labelSource.equals(labelTarget) && labelSource.equals(AMath.PHI))
            {
                Point2D.Double[] sourceCorners = getUserCorners();
                Point2D.Double[] targetCorners = targetWindow.getUserCorners();
                for(int i = 0; i < sourceCorners.length; ++i)
                    targetCorners[i].setLocation(targetCorners[i].getX(),
                                                 sourceCorners[i].getY());
                targetWindow.setUserCorners(targetCorners);
                AOutput.append("Copy " + AMath.PHI + " Scale from " + getName() +
                               " to " + targetWindow.getName() + "\n",
                               ALogInterface.NORMAL);
            }
        }
        else if(dragAction == dragActionType.COPY_HORIZONTAL_SCALE)
        {
            String labelSource = ((AScaleBorder) getBorder()).
                                 getHorizontalAxisLabel();
            String labelTarget = ((AScaleBorder) targetWindow.getBorder()).
                                 getHorizontalAxisLabel();
            if(labelSource.equals(labelTarget) && labelSource.equals("Z"))
            {
                Point2D.Double[] sourceCorners = getUserCorners();
                Point2D.Double[] targetCorners = targetWindow.getUserCorners();
                for(int i = 0; i < sourceCorners.length; ++i)
                    targetCorners[i].setLocation(sourceCorners[i].getX(),
                                                 targetCorners[i].getY());
                targetWindow.setUserCorners(targetCorners);
                AOutput.append("Copy Z Scale from " + getName() + " to " +
                               targetWindow.getName() + "\n",
                               ALogInterface.NORMAL);
            }
        }
    }


    private void setLocked(boolean locked)
    {
        if(locked == this.locked)return;
        boolean wasLocked = this.locked;
        this.locked = locked;
        ACanvas.getCanvas().fireLockChange();
        if(wasLocked && !locked)
            this.repaintFromScratch();
        ACanvas.getCanvas().fireLockChange();
    }


    public boolean isLocked()
    {
        return locked;
    }
    
    public JComponent getView() {
        return m_view;
    }
    
    public void invalidateQuietly() {
        // When you get told to paint in OpenGL you better paint otherwise
        // you're going to get your empty backbuffer painted to screen a lot
        // of the time ... could fix this by managing our buffer swaps by hand
        // see AWindowGLView.display() for more... - Adam
        bufferValid = false;
        borderValid = false;
    }
}

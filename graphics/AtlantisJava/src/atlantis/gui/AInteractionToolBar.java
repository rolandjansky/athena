package atlantis.gui;

import java.awt.FlowLayout;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import java.lang.reflect.Constructor;
import java.util.Hashtable;
import java.util.Vector;

import javax.swing.ButtonGroup;
import javax.swing.ImageIcon;
import javax.swing.JPanel;
import javax.swing.JPopupMenu;
import javax.swing.JToggleButton;
import javax.swing.JToolBar;

import org.w3c.dom.Node;
import org.w3c.dom.NodeList;

import atlantis.canvas.ACanvas;
import atlantis.canvas.AWindow;
import atlantis.globals.AGlobals;
import atlantis.graphics.layout.AFlowLayout;
import atlantis.interactions.AInteractionGroup;
import atlantis.interactions.AInteractionsConfigReader;
import atlantis.interactions.AInteractionsManager;
import atlantis.output.AExceptionHandler;
import atlantis.projection.AProjectionsManager;
import atlantis.utils.AUtilities;
import javax.swing.*;

/**
 * The ToolBar that appears in the GUI and shows the set of interactions for a
 * window. 
 * @author maillard
 */

public class AInteractionToolBar extends JToolBar {
  // the interactionsmanager this toolbar talks to
  private AInteractionsManager iManager;
  
  // currently selected group
  private AInteractionGroup currentGroup;
  
  /* previously selected group, needed for the history feature:
   * when we select a global interaction (eg SyncCursors), then all windows will use it.
   * If then we deselect it, all the windows will go back to their previous interactions */
  private AInteractionGroup previousGroup;
  
  // a panel that displays the additional controls needed by some interaction groups
  private JPanel controls;
  
  // base path for all radio button icons
  private final static String iconBasePath =
		  AGlobals.instance().getHomeDirectory()+"img"+System.getProperty("file.separator")+"interaction_";
  
  // button group for all radio buttons
  private ButtonGroup buttonGroup;
  
  // vector containing all interactionsgroups belonging to this toolbar
  Vector<AInteractionGroup> groups;

  // make the pool for holding the Interactions Control and the projections Menus
  private static Hashtable<String, Hashtable<String, AInteractionToolBar>> interactionControlsPool =
		  new Hashtable<String, Hashtable<String, AInteractionToolBar>>(20);

  public static Hashtable<String, Hashtable<String, JPopupMenu>> interactionMenusPool =
		  new Hashtable<String, Hashtable<String, JPopupMenu>>(20);

  /**
   * Creates an InteractionToolBar to work with a particular
   * InteractionsManager.
   * 
   * @param iManager The InteractionsManager.
   */
  public AInteractionToolBar(AInteractionsManager iManager) {
    // call parent constructor
    super();
    this.setLayout(new AFlowLayout(5, 5));

    //make the toolbar non-detachable (can not be detachable
    //as long as it does not sit inside an BorderLayout)
    this.setFloatable(false);
    this.setOpaque(false);

    this.iManager = iManager;

    groups = new Vector<AInteractionGroup>();
    controls = new JPanel();
    buttonGroup = new ButtonGroup();
  }

  /**
   * Returns the InteractionsManager associated with this InteractionToolBar.
   * 
   * @return The InteractionsManager
   */
  public AInteractionsManager getInteractionsManager() {
    return iManager;
  }

  /**
   * Called each time by the InteractionsManager when the ToolBar is connected
   * to it. Should be used to perform some initializations.
   */
  public void connect() {
    iManager.setContext(getSelectedGroup());
    if (getSelectedGroup() != null)
      AMouseHelpDialog.getInstance().processInteractionChange(getSelectedGroup().getInteractions());
  }

  /**
   * Adds the group to the InteractionToolBar. The group can
   * register some interactions.
   * @param group the group to be added.
   */
  public void addGroup(AInteractionGroup group, String toolTipText) {
    groups.addElement(group);
    final String groupName = group.getGroupName();
    
    // set up a button for the interaction group
    String iconPath = iconBasePath + groupName.toLowerCase() + ".png";

    ImageIcon newIcon = AUtilities.getFileAsImageIcon(iconPath);
    JToggleButton newButton = new JToggleButton(newIcon);
    newButton.setName(group.getGroupName());
    newButton.setToolTipText(toolTipText);

    // add the action listener
    newButton.addActionListener(new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        buttonClicked(groupName);
      }
    });
    
    // add the mouse listener for the help system
    newButton.addMouseListener(new MouseAdapter() {
      public void mousePressed(MouseEvent e) {
        // if the user right clicks on a button
        if (AUtilities.isRightMouseButton(e)) {
          JToggleButton butt = (JToggleButton) e.getSource();       
          // find the corresponding interactions group
          if(findGroup(butt.getName()) != null) {
            String className = findGroup(butt.getName()).getClass().getName();
            String temp = className.substring(className.lastIndexOf('.') + ".A".length());
            className = temp.substring(0, temp.length() - "Group".length());
            // show the help page for the group
            AHelpSystem.getInstance().showPage(className);
          }
        }
      }
    });
    
    // remove the controls panel so that the new button will
    // not appear after it
    this.remove(controls);

    // add the button to the ButtonGroup and to the ToolBar
    buttonGroup.add(newButton);
    this.add(newButton);
    
    // now we can put the controls panel back
    this.add(controls,-1);
    
    // repaint the toolbar
    this.validate();
    this.repaint();
  }

  /**
   * Returns an array containing all the groups registered in this ToolBar.
   * @return the groups
   */
  public AInteractionGroup[] getGroups() {
    return (AInteractionGroup[]) groups.toArray(new AInteractionGroup[0]);
  }

  /**
   * Returns the current interactionsgroup.
   * @return the selected group.
   */
  public AInteractionGroup getSelectedGroup() {
    return currentGroup;
  }

  /**
   * Returns the name of the current interactionsgroup.
   * 
   * @return the selected group name.
   */
  public String getSelectedGroupName() {
    return currentGroup.getGroupName();
  }

  /**
   * Sets the interactionsgroup given its name, by
   * performing a click on it.
   * @param groupName the name of the group.
   */
  public void setSelectedGroup(String groupName) {
    // loop through the interaction groups
    for(AInteractionGroup iG : groups) {
      // find the one with the corresponding name
      if(iG.getGroupName().equals(groupName)) {
        // click the corresponding button
        for(int i = 0; i < groups.size(); i++) {
          if(this.getComponentAtIndex(i) instanceof JToggleButton 
              && this.getComponentAtIndex(i).getName().equals(groupName)) {
            ((JToggleButton) this.getComponentAtIndex(i)).doClick();
            return;
          }
        }
      }
    }
  }
  
  /**
   * Returns a group based on its name
   * @return the interactionsgroup (if found) or null
   */
  public AInteractionGroup findGroup(String screenName) {
    for(AInteractionGroup iG : groups) {
      if(iG.getGroupName().equals(screenName)) return iG;
    }
    return null;
  }

  /**
   * Activate the default interactionsgroup (the first one)
   */
  public void setDefaultGroup() {
    // if there are any interaction groups
    if (groups.size() > 0) {
      // select the first one
      setSelectedGroup(groups.elementAt(0).getGroupName());
    }
  }

  /**
   * Restore the previously selected group, if there was one.
   */
  public void restorePreviousGroup() {
    if (previousGroup != null)
      this.setSelectedGroup(previousGroup.getGroupName());
  }

  /**
   * This gets called when one of the buttons in the toolbar
   * is clicked. It notifies the InteractionManager.
   * @param groupName the name of the InteractionGroup to be activated
   */
  public void buttonClicked(String groupName) {
    AInteractionGroup deselectedGroup = currentGroup;
    // loop through the groups
    for(AInteractionGroup iG : groups) {
      // find the one with the corresponding name
      if(iG.getGroupName().equals(groupName)) {
        // notify the interactionsmanager of the change
        iManager.setContext(iG);
        
        // remove the old additional controls panel, add the new one
        this.remove(controls);
        if(iG.hasAdditionalControls()) controls = iG.getAdditionalControls(); 
        else controls = new JPanel();
        this.add(controls,-1);
        if (this.getParent() != null) {
            this.getParent().validate();
        }
        
        // take care of the history (previousGroup remembers the last non-global group)
        AMouseHelpDialog.getInstance().processInteractionChange(iG.getInteractions());
        if (currentGroup != null && currentGroup.isWindowGroup()) {
          previousGroup = currentGroup;
        }
        currentGroup = iG;
       
        if(ACanvas.getCanvas().getCurrentWindow() != null)
          // if the interactionsgroup is global, update the other windows
          if ((iG.isCanvasGroup()) && iManager.getWindow().isCurrent()) {
            String[] wName = ACanvas.getCanvas().getCurrentLayout().getWindowNames();
            for (int i = 0; i < wName.length; i++)
              if (!wName[i].equals(iManager.getWindow().getName())) {
                ACanvas.getCanvas().getWindow(wName[i])
                  .getInteractionToolBar().setSelectedGroup(iG.getGroupName());
              }        
          } 
        // if a global interactionsgroup was deselected, restore the
        // previous groups in all the other windows
        if(deselectedGroup != null && deselectedGroup.isCanvasGroup() 
              && iManager.getWindow().isCurrent()) {
            String[] wName = ACanvas.getCanvas().getCurrentLayout().getWindowNames();
            for (int i = 0; i < wName.length; i++)
              if (!wName[i].equals(iManager.getWindow().getName()))
                  ACanvas.getCanvas().getWindow(wName[i])
                    .getInteractionToolBar().restorePreviousGroup();
          }
      }
    }
  }

  /**
   * Used to get the InteractionControl object for a specific window and projection.
   * @param wName The name of the window
   * @param pName The name of the projection
   * @return The AInteractionToolBar for that window and projection
   */
  public static AInteractionToolBar getInteractionToolBar(String wName, String pName) {
	  if(!ACanvas.getCanvas().isValidWindowName(wName))
		  throw new Error("Invalid window name");

	  if(!AProjectionsManager.isValidProjection(pName))
		  throw new Error("Invalid projection name");

	  Hashtable<String, AInteractionToolBar> list = interactionControlsPool.get(wName);

	  if(list==null) {
		  list=new Hashtable<String, AInteractionToolBar>(10);
		  interactionControlsPool.put(wName, list);
	  }

	  AInteractionToolBar control=list.get(pName);

	  if(control==null) {
		  control=makeInteractionToolBar(ACanvas.getCanvas().getWindow(wName),
				  AInteractionsConfigReader.getNode(pName));
		  // if there really existed an InteractionControl register it.
		  if(control!=null)
			  list.put(pName, control);
	  }

	  return control;
  }

  /**
   * Returns the Interaction's PopupMenu.
   * @param window
   * @return the new popup menu object
   */
  public static JPopupMenu getInteractionMenu(AWindow window) {
	  String wName=window.getName();
	  String pName=window.getProjection().getName();

	  if(!ACanvas.getCanvas().isValidWindowName(wName))
		  throw new Error("Invalid window name");

	  if(!AProjectionsManager.isValidProjection(pName))
		  throw new Error("Invalid projection name");

	  Hashtable<String, JPopupMenu> list=interactionMenusPool.get(wName);

	  if(list==null) {
		  list=new Hashtable<String, JPopupMenu>(10);
		  interactionMenusPool.put(wName, list);
	  }

	  JPopupMenu menu=list.get(pName);

	  if(menu==null) {
		  menu=makePopupMenu(getInteractionToolBar(wName, pName));
		  list.put(pName, menu);
	  }

	  return menu;
  }

  /** Creates the AInteractionToolBar and returns a pointer to it
   * 
   * @param window
   * @param node
   * @return the interaction tool bar
   */
  public static AInteractionToolBar makeInteractionToolBar(AWindow window, Node node) {
	  // check whether we really got a Node with information. If not return null;
	  if(node==null) return null;

	  AInteractionToolBar root=new AInteractionToolBar(window.getInteractionManager());

	  NodeList childs=node.getChildNodes();

	  for(int i=0; i<childs.getLength(); i++) {
		  Node child=childs.item(i);

		  if(child.getNodeType()==Node.ELEMENT_NODE) {
			  String fileName=child.getAttributes().getNamedItem("fileName").getNodeValue();
			  String screenName=child.getAttributes().getNamedItem("screenName").getNodeValue();
			  String toolTip=child.getAttributes().getNamedItem("toolTip").getNodeValue();

			  if(child.getNodeName().equals("Panel"))
				  root.addGroup(makeInteractionGroup(window, fileName, screenName), toolTip);
		  }
	  }

	  //Also select the default interaction group
	  root.setDefaultGroup();

	  return root;
  }

  private static AInteractionGroup makeInteractionGroup(
		  AWindow window, String fileName, String screenName) {
	  Object group=null;

	  try {
		  Constructor[] c=Class.forName("atlantis.interactions.A"+fileName+"Group").getDeclaredConstructors();

		  group=c[0].newInstance(new Object[] {window.getInteractionManager()});
	  } catch(Exception e) {
		  e.printStackTrace();
		  AExceptionHandler.processException("Cannot make interaction group "+"A"+fileName+"Group", e);
	  }
	  ((AInteractionGroup)group).setGroupName(screenName);
	  return(AInteractionGroup)group;
  }

  private static JPopupMenu makePopupMenu(AInteractionToolBar root) {
	  JPopupMenu menu=new JPopupMenu();

	  AInteractionGroup[] panels=root.getGroups();

	  for(int i=0; i<panels.length; i++)
		  menu.add(panels[i].getPopupItem());

	  return menu;
  }

}

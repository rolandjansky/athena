package atlantis.interactions;

import atlantis.canvas.ACanvas;
import atlantis.parameters.APar;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import javax.swing.JMenuItem;
import javax.swing.JPanel;
import java.util.Vector;


/**
 * The InteractionGroup groups interactions of the same kind under a single
 * object. The InteractionToolBar deals with the InteractionGroups instead
 * of dealing with the interactions directly.
 */
public abstract class AInteractionGroup {
  protected static APar parameterStore = APar.instance();

  /** A group type which once made current affects only the window in which it works*/
  public static final int WINDOW_GROUP=0;

  /** A group type which once made current affects all the windows of the Canvas*/
  public static final int CANVAS_GROUP=1;

  protected String name;
  protected String toolTip;
  protected AInteractionsManager iManager;
  private int groupType;
  private boolean isCurrent=false;
  private Vector<AInteraction> interactions;

  /**
   * Creates an InteractionGroup of a specific type
   * and connected to some InteractionsManager
   * @param groupType The type of the interface. Can be: WINDOW_GROUP or CANVAS_GROUP
   * @param iManager The InteractionsManager to which this group will be connected.
   */
  public AInteractionGroup(int groupType, AInteractionsManager iManager) {

    this.groupType=groupType;
    this.iManager=iManager;

    interactions=new Vector<AInteraction>();
  }

  public void setGroupName(String name) {
    this.name=name;
  }

  public String getGroupName() {
    return name;
  }

  public void setToolTip(String toolTip) {
    this.toolTip=toolTip;
  }

  public String getToolTip() {
    return toolTip;
  }

  public JMenuItem getPopupItem() {
    JMenuItem item=new JMenuItem(name);

    item.addActionListener(new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        ACanvas.getCanvas().getCurrentWindow().getInteractionToolBar().setSelectedGroup(name);
      }
    });

    return item;
  }

  /**
   * Called by the InteractionsManager when the group gets connected to it.
   */
  public void connect() {}

  /**
   * Called by the InteractionsManager when the group gets disconnected from it.
   */
  public void disconnect() {}

  public boolean isWindowGroup() {
    return(groupType==WINDOW_GROUP);
  }

  public boolean isCanvasGroup() {
    return(groupType==CANVAS_GROUP);
  }
  
  /**
   * Checks if any additional controls are provided by the current interaction group
   * @return true if the interaction group has additional controls
   */
  public boolean hasAdditionalControls() { return false; }
  
  /**
   * Returns the additional controls for the current interaction group
   * @return the panel containing the additional controls
   */
  
  public  JPanel getAdditionalControls() { return new JPanel(); }

  /**
   * Registers a new interaction which will be available with this group.
   * @param i The Interaction to be registered.
   */
  protected void addInteraction(AInteraction i) {
    interactions.addElement(i);
    if(isCurrent)
      iManager.setContext(this);
  }

  /**
   * Removes a previously registered interaction from the group.
   * @param i The Interaction to be removed.
   */
  protected void removeInteraction(AInteraction i) {
    interactions.remove(i);
  }

  /**
   * Removes all previously registered interactions from the group.
   */
  protected void removeAllInteractions() {
    interactions.clear();
  }

  public Vector<AInteraction> getInteractions() {
    return interactions;
  }

  /**
   * Used to help copying of group. The group should set its internal state to be
   * the same as the state of the given group.
   * @param givenGroup The group to be copied.
   */
  public void setGroupState(AInteractionGroup givenGroup) {}

}

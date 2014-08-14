package atlantis.interactions;

import atlantis.graphics.ACursorFactory;
import atlantis.parameters.AParameter;
import atlantis.parameters.APar;
import atlantis.projection.*;
import java.awt.Graphics2D;
import java.awt.event.ActionEvent;
import java.awt.event.KeyEvent;
import java.awt.event.ActionListener;
import javax.swing.JMenuItem;
import java.awt.geom.*;
import java.util.Vector;

/**
 * The FishEye transformation will expand the central part of the display,
 * while keeping the outer borders. This helps in e.g. expanding the inner detector region.
 **/

public class AFishEyeInteraction extends AInteraction
    implements ASleepMouseDragListener, ActionListener, AEnterExitListener {

  //A list of listeners for fishEye changes
  private Vector<AFishEyeChangeListener> fishEyeChangeListeners = new Vector<AFishEyeChangeListener>();

  private final int d=20;
  private final int region = 0;
  private Point2D.Double firstP;
  private JMenuItem[] popupItems;

  private final static String TOGGLE_FISHEYE="Fish Eye On/Off";
 
  /**
   * Create a new FishEye interaction
   */
  public AFishEyeInteraction() {
    super(1, WINDOW_TOTAL_REPAINT, WORLD_COORDINATES, false);

    hr[0]=new Ellipse2D.Double(0, 0, d, d);

    popupItems = new JMenuItem[] {
      new JMenuItem(TOGGLE_FISHEYE),
    };
    
    for (int i = 0; i < popupItems.length; i++) {
        popupItems[i].addActionListener(this);
    }
  }

 
  /**
   * Initialize FishEye transformation using an inital point
   */
  public int init(Point2D.Double p, int key) {
      
    AProjection2D projection=(AProjection2D)window.getProjection();

    firstP=new Point2D.Double(Math.abs(p.x), Math.abs(p.y));
    firstP=projection.inverseNonLinearTransform(firstP);

    return region;
  }

  /** 
   * Returns the button this interaction uses
   **/
  public int getButton() {
    return AInteractionsManager.LEFT;
  }

  /**
   * Get projection parameters using APar
   **/
  private AParameter getParameter(String name) {
    AProjection projection=window.getProjection();

    if(projection instanceof AProjectionYX)
      return parameterStore.get("YX", name);
    else if(projection instanceof AProjectionRZ)
      return parameterStore.get("RZ", name);
    else if(projection instanceof AProjection3D)
      return parameterStore.get("3D", name);
    else if(projection instanceof AProjectionYZ||projection instanceof AProjectionXZ)
      return parameterStore.get("XZ", name);
    else if(projection instanceof AProjectionFR)
      return parameterStore.get("FR", name);
    else if(projection instanceof AProjectionFZ)
      return parameterStore.get("FZ", name);
    else
      throw new Error("Fish Eye should be used ONLY in YX, RZ, FZ and FR");
  }

  /**
   * Get new FishEye parameter if mouse has moved to p in YX projection
   **/
  private void updateYX(Point2D.Double p, int key) {
    double r1=Math.sqrt(firstP.x*firstP.x+firstP.y*firstP.y);
    double r2=Math.sqrt(p.x*p.x+p.y*p.y);
    double rTo=getParameter("rTo").getD();

    if(r1<rTo&&r2>rTo) return;
    if(r2<rTo&&r1>rTo) return;

    double newFishEyeValue=1000*(r2-r1)/(r1*rTo-r1*r2);

    if(newFishEyeValue<0||newFishEyeValue>25) return;

    setValue(newFishEyeValue);
    fireFishEyeChange(getStatus(),getValue());
  }

  /**
   * Get new FishEye parameter if mouse has moved to p in RZ projection
   **/
  private void updateRZ(Point2D.Double p, int key) {
    double newFishEyeValue=0;

    switch(key) {
    case KeyEvent.VK_UNDEFINED:
    case KeyEvent.VK_F:
    case KeyEvent.VK_H:
      double z1=firstP.x;
      double z2=Math.abs(p.x);
      double zTo=getParameter("zTo").getD();

      if(z1<zTo&&z2>zTo) return;
      if(z2<zTo&&z1>zTo) return;

      newFishEyeValue=1000*(z2-z1)/(z1*zTo-z1*z2);
      break;

    case KeyEvent.VK_V:
      double r1=Math.abs(firstP.y);
      double r2=Math.abs(p.y);
      double rTo=getParameter("rTo").getD();

      if(r1<rTo&&r2>rTo) return;
      if(r2<rTo&&r1>rTo) return;

      newFishEyeValue=1000*(r2-r1)/(r1*rTo-r1*r2);
      break;
    }

    if(newFishEyeValue<0||newFishEyeValue>25) return;
    setValue(newFishEyeValue);
    fireFishEyeChange(getStatus(),getValue());
  }

  /**
   * Get new FishEye parameter if mouse has moved to p in XZ projection
   **/
  private void updateXZ(Point2D.Double p, int key) {
    double newFishEyeValue=0;

    switch(key) {
    case KeyEvent.VK_UNDEFINED:
    case KeyEvent.VK_F:
    case KeyEvent.VK_H:
      double z1=firstP.x;
      double z2=Math.abs(p.x);
      double zTo=getParameter("zTo").getD();

      if(z1<zTo&&z2>zTo) return;
      if(z2<zTo&&z1>zTo) return;

      newFishEyeValue=1000*(z2-z1)/(z1*zTo-z1*z2);
      break;

    case KeyEvent.VK_V:
      double r1=Math.abs(firstP.y);
      double r2=Math.abs(p.y);
      double rTo=getParameter("yTo").getD();

      if(r1<rTo&&r2>rTo) return;
      if(r2<rTo&&r1>rTo) return;

      newFishEyeValue=1000*(r2-r1)/(r1*rTo-r1*r2);
      break;
    }

    if(newFishEyeValue<0||newFishEyeValue>25) return;
    setValue(newFishEyeValue);
    fireFishEyeChange(getStatus(),getValue());
  }

  /**
   * Get new FishEye parameter if mouse has moved to p in Phi-R projection
   **/
  private void updateFR(Point2D.Double p, int key) {
    double r1=firstP.x;
    double r2=p.x;
    double rTo=getParameter("rTo").getD();

    if(r1<rTo&&r2>rTo) return;
    if(r2<rTo&&r1>rTo) return;

    double newFishEyeValue=1000*(r2-r1)/(r1*rTo-r1*r2);

    if(newFishEyeValue<0||newFishEyeValue>25) return;

    setValue(newFishEyeValue);
    fireFishEyeChange(getStatus(),getValue());
  }

  /**
   * Get new FishEye parameter if mouse has moved to p in Phi-Z projection
   **/
  private void updateFZ(Point2D.Double p, int key) {
    double z1=Math.abs(firstP.x);
    double z2=Math.abs(p.x);
    double zTo=getParameter("zTo").getD();

    if(z1<zTo&&z2>zTo) return;
    if(z2<zTo&&z1>zTo) return;

    double newFishEyeValue=1000*(z2-z1)/(z1*zTo-z1*z2);

    if(newFishEyeValue<0||newFishEyeValue>25) return;

    setValue(newFishEyeValue);
    fireFishEyeChange(getStatus(),getValue());
  }

  /**
   * Change FishEye parameter for corresponding projection
   * if mouse is draged
   **/
  public void drag(Point2D.Double p, int region, int key) {
    if(!getParameter("FishEye").getStatus()) return;

    AProjection proj=window.getProjection();

    if(proj instanceof AProjectionYX)
      updateYX(p, key);
    else if(proj instanceof AProjectionRZ||proj instanceof AProjection3D)
      updateRZ(p, key);
    else if(proj instanceof AProjectionXZ||proj instanceof AProjectionYZ)
      updateXZ(p, key);
    else if(proj instanceof AProjectionFR)
      updateFR(p, key);
    else if(proj instanceof AProjectionFZ)
      updateFZ(p, key);
  }
  
  // Empty interface implementations
  public void stop() {}
  public void start(Point2D.Double p, int region, int key) {}
  public void cancel() {}
  public void paint(Graphics2D g) {}

  public int getPopupType() {
    return APopupListener.WINDOW_POPUP;
  }

  /*public void showPopup(Point p, int region) {
    popupMenu.show(window, p.x, p.y);
  }*/

  public JMenuItem[] getPopupItems() {
      return popupItems;
  }
  

  /**
   * Implementaiton of the ActionListener
   * @param e 
   */
  public void actionPerformed(ActionEvent e) {
    String action=e.getActionCommand();

    if(action.equals(TOGGLE_FISHEYE)) {
      //toggle the fisheye status
      setStatus(!getStatus());
      //Update the window
      window.repaintFromScratch();
      //And the GUI
      fireFishEyeChange(getStatus(),getValue());
    }
  }

  /**
   * Public method so it can be called from the AFishEyeGroup to change the
   * fish eye amount to a fixed value
   * @param value the new value
   */
  public void setValue(double value){
    getParameter("FishEye").setD(value);
  }

  /**
   * Return FishEye value (e.g to update the FishEye panel)
   * @return the fishEye value
   */
  public double getValue(){
    return getParameter("FishEye").getD();
  }

  /**
   * Public method so it can be called externally from the AFishEyeGroup 
   * @param status true or false
   */
  public void setStatus(boolean status) {
    getParameter("FishEye").setStatus(status);
  }

  /** 
   * @return the current fishEye status(true or false)
   */
  public boolean getStatus() {
    return getParameter("FishEye").getStatus();
  }  
  
  /**
   * Gets called each time we enter a window in which this interaction is active
   */
  public void entered(){
      //Set mouse cursor
      window.setCursor(ACursorFactory.getInstance().getFishEyeCursor());
  }
   
  /**
   * Gets called each time we leave a window in which this interaction is active
   */
  public void exited(){
     //Set mouse cursor
     window.setCursor(ACursorFactory.getInstance().getDefaultCursor());
  }

  /**
   * Return modifier keys for this projection
   * - F for Fast (only detectors are update)
   * - V for react only to Vertical mouse movement (only in RZ and XZ projection)
   * - H for react only to Horizontal mouse movement (only in RZ and XZ projection, default)
   **/
  public AModifier[] getModifiers(){
    if(window.getProjection() instanceof AProjectionRZ ||
       window.getProjection() instanceof AProjectionXZ) {
      return new AModifier[] {
          new AModifier(KeyEvent.VK_F, false, "Fast (data not drawn)"),
          new AModifier(KeyEvent.VK_V, false, "Vertically defined FishEye"),
          new AModifier(KeyEvent.VK_UNDEFINED, false, "Horizontally defined FishEye"),
          new AModifier(KeyEvent.VK_H, false, "Horizontally defined FishEye")
      };
    } else {

      return new AModifier[] {
          new AModifier(KeyEvent.VK_F, false, "Fast (data not drawn)")  
      };
    }
  }
  
  /**
   * Add a new listener for FishEyeChange incidents
   * @param listener to be added to list
   */
  public void addFishEyeChangeListener(AFishEyeChangeListener listener) {
    //Add this listener
    fishEyeChangeListeners.addElement(listener);
    //And bring it up to date
    listener.fishEyeChange(getStatus(), getValue());
  
  }

  /**
   * Remove a listener from the liste
   * @param listener to be removed from list
   */
  public void removeFishEyeChangeListener(AFishEyeChangeListener listener) {
    //Remove this listener
    fishEyeChangeListeners.removeElement(listener);
  }

  /**
   * Call fishEyeChange method of all FishEyeChange listeners
   * @param status the new fisheye status
   * @param value the new fisheye value
   */
  private void fireFishEyeChange(boolean status, double value) {

    // Loop over all listeners
    for (AFishEyeChangeListener listener : fishEyeChangeListeners) 
    {
      // give them the new status
      listener.fishEyeChange(status,value);
    }
  }
  
}


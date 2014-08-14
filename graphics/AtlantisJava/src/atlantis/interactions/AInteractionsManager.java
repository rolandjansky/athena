package atlantis.interactions;

import java.awt.Rectangle;
import java.awt.event.KeyEvent;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.event.MouseMotionListener;
import java.awt.event.WindowFocusListener;
import java.awt.event.WindowEvent;
import java.awt.geom.Point2D;
import java.awt.geom.RectangularShape;
import java.util.LinkedList;
import java.util.Vector;

import javax.swing.JPopupMenu;
import javax.swing.SwingUtilities;
import javax.swing.JMenuItem;

import atlantis.canvas.ACanvas;
import atlantis.canvas.AWindow;
import atlantis.gui.AEventQueue;
import atlantis.gui.AInteractionToolBar;
import atlantis.gui.APointerPositionWindow;
import atlantis.gui.APreferencesControl;
import atlantis.projection.AProjectionTrackResidual;
import atlantis.projection.AProjectionLegoPlot;
import atlantis.projection.AProjection2D;

/**
 * The InteractionsManager connects to the corresponding window in order to
 * provide basic functionality to the interactions.
 */
public class AInteractionsManager
implements MouseListener, MouseMotionListener, WindowFocusListener {
    
    public final static int LEFT=0;
    public final static int MIDDLE=1;
    public final static int RIGHT=2;
    public final static int ALL=3;
    
    private final static int safetySize=5;
    
    private AWindow window;            //The window that is monitored by this interaction manager
    private LinkedList<AInteraction> interactions;   //A list of all the interactions linked to this manager
    private AInteraction dragInter;
    private int dragInterHotRegion;
//    private AInteraction mcInteraction;  //This one is set but not used by anything
//    private int mcRegion;                //This one is set but not used by anything
    private RectangularShape[] old_hr;
    private AInteractionGroup previousInterface;
    private AModifier[] mouseModifiers;
    private boolean origAlias=false;
    
    /**
     * Creates an InteractionsManager to work with a specific window.
     * @param _window The window with wich the InteractionsManager should work.
     */
    public AInteractionsManager(AWindow _window) {
        this.window=_window;
        
        interactions=new LinkedList<AInteraction>();
//        mcInteraction=null;
//        mcRegion=-1;
        
        window.addMouseListener(this);
        window.addMouseMotionListener(this);
        
        mouseModifiers=new AModifier[] {
            new AModifier(KeyEvent.VK_O, true, "Output of pointer position"),
            new AModifier(KeyEvent.VK_W, true, "Window Menu pops up"),
            new AModifier(KeyEvent.VK_I, true, "Interaction Manager pops up"),
            new AModifier(KeyEvent.VK_UNDEFINED, true, "Interaction Menu pops up")
        };
    }
    
    public AModifier[] getMouseModifiers() {
        return mouseModifiers;
    }
    
    // Implementation of MouseListener
    
    public void mousePressed(MouseEvent e) {
        
        if(!ACanvas.getCanvas().getCurrentWindow().equals(window))
            ACanvas.getCanvas().setCurrentWindow(window.getName());
        
        if(AInteractionsManager.isIntRightMouseButton(e))
            switch(AEventQueue.getKeyboardState()) {
                    
                case KeyEvent.VK_W:
                    if(!(window.getProjection() instanceof AProjectionTrackResidual))
                        AWindow.getPopupMenu().show(window, e.getX(), e.getY());
                    return;
                    
                case KeyEvent.VK_I:
                    JPopupMenu menu=AInteractionToolBar.getInteractionMenu(window);
                    
                    menu.show(window, e.getX(), e.getY());
                    return;
                    
                case KeyEvent.VK_O:
                    Point2D.Double position =window.calculateUser(e.getX(),e.getY());
                    //If LegoPlot need to alter phi coord using reverse of the adjustPhi function
                    if( window.getProjection() instanceof AProjectionLegoPlot)
                        position.x=-AProjectionLegoPlot.adjustPhi(window,-position.x,position.y);
                    //Apply inverse non-linear transforms, if in 2D projection 
                    if ( window.getProjection() instanceof AProjection2D)
                      position = ((AProjection2D)window.getProjection()).inverseNonLinearTransform(position);
                    //Add labels
                    String positionX = window.getProjection().getXLabel() + parseUnits(position.x, window.getProjection().getXUnits());
                    String positionY = window.getProjection().getYLabel() + parseUnits(position.y, window.getProjection().getYUnits());
                    APreferencesControl.setPosMenuItem(true);//shows window if not already visible
                    APointerPositionWindow.getInstance().toFront();//brings to front
                    APointerPositionWindow.append(positionX + ", " + positionY + "\n");
                    return;
            }
        
        processMousePressed(e);
        
        if(dragInter!=null) {
            dragInter.hr=old_hr;
            ((AMouseDragListener)dragInter).cancel();
//            if(dragInter instanceof AEnterExitListener)
//                ((AEnterExitListener)dragInter).exited(dragInterHotRegion);
            repaintInteraction(dragInter);
            dragInter=null;
            return;
        }
        
        if(AInteractionsManager.isIntRightMouseButton(e))
            if(processPopupShow(e))
                return;
        
        if(setupDraggingByHotRegion(e)) return;
        if(initializeSleepingInteraction(e)) return;
    }
    
    private boolean processPopupShow(MouseEvent e) {
        for(int i=interactions.size()-1; i>=0; i--) {
            AInteraction interaction=(AInteraction)interactions.get(i);
            
            for(int region=0; region<interaction.hr.length; region++) {
                if(interaction.getScreenHotRegion(region).contains(e.getPoint())) {
                    interaction.showPopupMenu(e.getPoint(), region);
                    return true;
                }
            }
        }
        
        for(int i=interactions.size()-1; i>=0; i--) {
            AInteraction interaction=(AInteraction)interactions.get(i);
            
            //Turn off right click menu for pick interaction
            JMenuItem[] projmenu = (interaction instanceof APickInteraction) ? null : window.getProjection().getPopupItems();
            JMenuItem[] intmenu = (interaction instanceof APickInteraction) ? null : interaction.getPopupItems();
            
            JPopupMenu finalmenu = new JPopupMenu();
                  
            // Combine projection and interaction menus     
            for (int it = 0; intmenu != null && it < intmenu.length; it++) {
                finalmenu.add(intmenu[it]);
            }
            
            if (projmenu != null && projmenu.length > 0 && 
                    intmenu != null && intmenu.length > 0) {
                
                finalmenu.addSeparator();
            }
            
            for (int it = 0; projmenu != null && it < projmenu.length; it++) {
                finalmenu.add(projmenu[it]);
            }
            
            if (finalmenu.getComponentCount() != 0) {
                finalmenu.show(window, (int)e.getPoint().getX(), (int)e.getPoint().getY());
            }
        }
        
        return false;
    }
    
    public void mouseReleased(MouseEvent e) {
        if(dragInter!=null) {
            ((AMouseDragListener)dragInter).stop();
//            if(dragInter instanceof AEnterExitListener)
//                ((AEnterExitListener)dragInter).exited(dragInterHotRegion);
            repaintInteraction(dragInter);
            dragInter=null;
        }
        //reset anti-alias
        if(origAlias && !(dragInter instanceof ASelection))
            APreferencesControl.setAliasMenuItem(true);
    }
    
    public void mouseClicked(MouseEvent e) {}
    
    /**
     * This function is called each time we enter the window this interaction manager is assigned to
     */
    public void mouseEntered(MouseEvent e) {
        
      //Loop over interaction list from the end
      for(int i=interactions.size()-1; i>=0; i--) {
        
        //get the interaction
        AInteraction interaction=(AInteraction)interactions.get(i);
        
        //Check if this one listens to Enter/Exit events
        if (interaction instanceof AEnterExitListener){           
          ((AEnterExitListener)interaction).entered();
        }
      }
    }
    
    /**
     * This function is called each time we exit the window this interaction manager is assigned to
     */
    public void mouseExited(MouseEvent e) {
 
      //Loop over interaction list from the end
      for(int i=interactions.size()-1; i>=0; i--) {
            
        //get the interaction
        AInteraction interaction=(AInteraction)interactions.get(i);
        
        //Check if this one listens to Enter/Exit events
        if (interaction instanceof AEnterExitListener) 
          ((AEnterExitListener)interaction).exited();
      }
    }
    
    private void processMousePressed(MouseEvent e) {
        //update if using anti-alias so can switch off during drag
        if(!(dragInter instanceof ASelection))
            origAlias = APreferencesControl.getAliasMenuItem();
        for(int i=interactions.size()-1; i>=0; i--) {
            AInteraction interaction=(AInteraction)interactions.get(i);
            
            if(!(interaction instanceof AMousePressListener)) continue;
            AMousePressListener clickListener=(AMousePressListener)interaction;
            
            if(clickListener.getPressButton()!=AInteractionsManager.ALL)
                if(clickListener.getPressButton()!=getButton(e)) continue;
            Point2D.Double p=null;
            
            switch(interaction.coordinatesType) {
                case AInteraction.SCREEN_COORDINATES:
                    p=new Point2D.Double(e.getX(), e.getY());
                    break;
                    
                case AInteraction.WORLD_COORDINATES:
                    // p = window.calculateUser(inverseNonLinearTransform(e.getPoint()));
                    p=window.calculateUser(e.getPoint());
                    break;
            }
            clickListener.pressed(p, getButton(e), AEventQueue.getKeyboardState());
            setTopZOrder(interaction);
            repaintInteraction(interaction);
        }
    }
    
    // Implementation of MouseMotionListener
    
    public void mouseDragged(MouseEvent e) {
        if(dragInter==null) return;
        
        //switch off anti-alias
        if(origAlias && !(dragInter instanceof ASelection))
            APreferencesControl.setAliasMenuItem(false);
        Point2D.Double p=new Point2D.Double(e.getX(), e.getY());
        
        if(dragInter.coordinatesType==AInteraction.WORLD_COORDINATES)
            // p = dragInter.manager.getWindow().calculateUser(inverseNonLinearTransform(p));
            p=window.calculateUser(p);
        
        ((AMouseDragListener)dragInter).drag(p, dragInterHotRegion, AEventQueue.getKeyboardState());
        repaintInteraction(dragInter);
    }
    
    //This function is called every time the mouse is moved on the window
    public void mouseMoved(MouseEvent e) {
    }
    
    // Implementation of WindowsFocusListener
    
    /**
     * Get called each time the canvas has obtained a focus
     */
    public void windowGainedFocus(WindowEvent e) {
        //Check if the mouse is over this window (or any of its children)
        if (getWindow().getMousePosition(true) != null){
          //raise a mouseEntered event, so the cursor gets updated
          this.mouseEntered(null);
        }
    }
    
    /**
     * Get called each time the canvas has lost a focus
     */
    public void windowLostFocus(WindowEvent e) {}  
    
    //Methods belonging to this class
    
    /**
     * Returns all the interactions associated with this InteractionsManager.
     * @return The LinkedList containing the interactions.
     * DO NOT CHANGE THIS LINKEDLIST, JUST READ IT.
     */
    public LinkedList<AInteraction> getInteractions() {
        return interactions;
    }
    
    public AWindow getWindow() {
        return window;
    }
    
    /**
     * This function disconnects the current pannel and all it's
     * interactions from the InteractionsManager.
     */
    public void forgetContext() {
        // Disconnect all the old interactions
        if(previousInterface!=null) previousInterface.disconnect();
        for(int i=0; i<interactions.size(); i++) {
            AInteraction inter=(AInteraction)interactions.get(i);
            
            inter.disconnect();
        }
        
        // Clear the list and repaint the window
        interactions.clear();
        window.repaint();
    }
    
    /**
     * Sets the current interaction interface and adds its interactions to the list
     * (removing first the old interface and its interactions).
     * @param iFace The new interface.
     */
    public void setContext(AInteractionGroup iFace) {
        forgetContext();
        
        if(iFace!=null)
        {
            // add to the list, connect and repaint the new interactions
            Vector<AInteraction> iFaceInter=iFace.getInteractions();
            
            for(int i=0; i<iFaceInter.size(); i++) {
                AInteraction inter=(AInteraction)iFaceInter.get(i);
                interactions.addLast(inter);
                inter.connect(this);
                repaintInteraction(inter);
            }
            
            iFace.connect();
        }
        previousInterface=iFace;
    }
    
    private void repaintInteraction(AInteraction interaction) {
        switch(interaction.repaintMode) {
            case AInteraction.NO_REPAINT:
                return;
                
            case AInteraction.ALL_WINDOWS_REPAINT:
                window.repaint();
                ACanvas.getCanvas().repaintOthers(window);
                return;
                
            case AInteraction.WINDOW_TOTAL_REPAINT:
                window.repaintFromScratch();
                return;
                
            case AInteraction.AUTOMATIC_REPAINT:
                Rectangle regionToRedraw=interaction.getScreenHotRegion(0).getBounds();
                
                for(int i=1; i<interaction.hr.length; i++)
                    regionToRedraw.add(interaction.getScreenHotRegion(i).getBounds());
                
                if((interaction.pt!=null)&&(interaction.pt.length!=0))
                    for(int i=0; i<interaction.pt.length; i++)
                        regionToRedraw.add(getScreenPointToInclude(interaction, i));
                
                regionToRedraw.x-=safetySize;
                regionToRedraw.y-=safetySize;
                regionToRedraw.width+=2*safetySize;
                regionToRedraw.height+=2*safetySize;
                
                if(interaction.oldAffectedRegion==null)
                    window.repaint(regionToRedraw);
                else
                    window.repaint(SwingUtilities.computeUnion(interaction.oldAffectedRegion.x,
                            interaction.oldAffectedRegion.y, interaction.oldAffectedRegion.width,
                            interaction.oldAffectedRegion.height, new Rectangle(regionToRedraw)));
                
                interaction.oldAffectedRegion=regionToRedraw;
                return;
        }
    }
    
    /**
     * This method converts the specified "point to include" from user
     * coordinates to screen coordinates.
     * If the point is already in screen coordinates it just returns it.
     */
    private Point2D.Double getScreenPointToInclude(AInteraction interaction, int n) {
        switch(interaction.coordinatesType) {
            case AInteraction.SCREEN_COORDINATES:
                return interaction.pt[n];
                
            case AInteraction.WORLD_COORDINATES:
                return window.calculateDisplay(interaction.pt[n]);
                
            default:
                return null;
        }
    }
    
    private boolean setupDraggingByHotRegion(MouseEvent e) {
        for(int i=interactions.size()-1; i>=0; i--) {
            AInteraction interaction=(AInteraction)interactions.get(i);
            
            if(!(interaction instanceof AMouseDragListener)) continue;
            AMouseDragListener dInter=(AMouseDragListener)interaction;
            
            if(dInter.getButton()!=getButton(e)) continue;
            
            for(int j=0; j<interaction.hr.length; j++) {
                if(interaction.getScreenHotRegion(j).contains(e.getPoint())) {
                    dragInter=interaction;
                    dragInterHotRegion=j;
                    
                    old_hr=new RectangularShape[dragInter.hr.length];
                    for(int region=0; region<dragInter.hr.length; region++)
                        old_hr[region]=(RectangularShape)dragInter.hr[region].clone();
                    
                    Point2D.Double p=new Point2D.Double(e.getX(), e.getY());
                    
                    if(interaction.coordinatesType==AInteraction.WORLD_COORDINATES)
                        p=window.calculateUser(p);
                    
                    // add this statement to guarantee "p0" in AZMRInteraction is initialized
                    // maybe need more analysis here
                    if(dragInter instanceof AZMRInteraction)
                        ((ASleepMouseDragListener)dragInter).init(p, AEventQueue.getKeyboardState());
                    
                    setTopZOrder(dragInter);
                    ((AMouseDragListener)dragInter).start(p, dragInterHotRegion, AEventQueue.getKeyboardState());
                    repaintInteraction(dragInter);
                    return true;
                }
            }
        }
        return false;
    }
    
    private boolean initializeSleepingInteraction(MouseEvent e) {
        for(int i=interactions.size()-1; i>=0; i--) {
            AInteraction interaction=(AInteraction)interactions.get(i);
            
            if(interaction instanceof ASleepMouseDragListener) {
                ASleepMouseDragListener sdListener=(ASleepMouseDragListener)interaction;
                
                if(sdListener.getButton()==getButton(e)) {
                    dragInter=interaction;
                    
                    old_hr=new RectangularShape[dragInter.hr.length];
                    for(int region=0; region<dragInter.hr.length; region++)
                        old_hr[region]=(RectangularShape)dragInter.hr[region].clone();
                    
                    Point2D.Double p=new Point2D.Double(e.getX(), e.getY());
                    
                    if(interaction.coordinatesType==AInteraction.WORLD_COORDINATES)
                        p=window.calculateUser(p);
                    
                    dragInterHotRegion=((ASleepMouseDragListener)dragInter).init(p, AEventQueue.getKeyboardState());
                    
                    setTopZOrder(dragInter);
                    ((AMouseDragListener)dragInter).start(p, dragInterHotRegion, AEventQueue.getKeyboardState());
//                    if(dragInter instanceof AEnterExitListener)
//                        ((AEnterExitListener)dragInter).entered(dragInterHotRegion);
                    
                    repaintInteraction(dragInter);
                    return true;
                }
            }
        }
        
        return false;
    }
    
    private void setTopZOrder(AInteraction ineraction) {
        interactions.remove(ineraction);
        interactions.addLast(ineraction);
        repaintInteraction((AInteraction)interactions.getLast());
    }
    
    private int getButton(MouseEvent e) {
        if(SwingUtilities.isLeftMouseButton(e))
            return LEFT;
        else if(SwingUtilities.isMiddleMouseButton(e))
            return MIDDLE;
        else if(AInteractionsManager.isIntRightMouseButton(e))
            return RIGHT;
        else
            return -1;
    }
    
	private static boolean isIntRightMouseButton(MouseEvent e)
	{
	    return SwingUtilities.isRightMouseButton(e) ||
	        AEventQueue.getIntRightMouseButton();
	}
	
    private String parseUnits(double d, String units)
    {
        //covert units and value units shown are cm by default
        double abs = Math.abs(d);
        if(units.equals("(cm)"))
        {
            if (abs >= 100.)
                return ": " + Math.rint(1000. * d / 100.) / 1000. + " m";
            else if (abs >= 1.)
                return ": " + Math.rint(1000. * d / 1.) / 1000. + " cm";
            else if (abs >= 0.1)
                return ": " + Math.rint(1000. * d / 0.1) / 1000. + " mm";
            else
                return ": " + Math.rint(d / 0.0001) + " um";
        }
        else
        {
            return ": " + Math.rint(1000. * d / 1.) / 1000. + units;
        }
    }
}

package atlantis.list;

import java.awt.Color;
import java.awt.Component;
import java.awt.Cursor;
import java.awt.Dimension;
import java.awt.Insets;
import java.awt.Point;
import java.awt.dnd.DragSource;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.event.MouseMotionListener;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Enumeration;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;
import java.util.Set;

import javax.swing.AbstractAction;
import javax.swing.Action;
import javax.swing.Box;
import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JPopupMenu;
import javax.swing.JScrollPane;
import javax.swing.JTree;
import javax.swing.tree.DefaultMutableTreeNode;
import javax.swing.tree.DefaultTreeCellRenderer;
import javax.swing.tree.DefaultTreeModel;
import javax.swing.tree.MutableTreeNode;
import javax.swing.tree.TreeNode;
import javax.swing.tree.TreePath;

import atlantis.canvas.ACanvas;
import atlantis.data.AHelix;
import atlantis.data.ARVxData;
import atlantis.data.ATrackData;
import atlantis.event.AData;
import atlantis.event.AEvent;
import atlantis.event.AEventManager;
import atlantis.globals.AGlobals;
import atlantis.graphics.ACursorFactory;
import atlantis.graphics.AIcon;
import atlantis.graphics.APickingGraphics2D;
import atlantis.graphics.colormap.AColorMap;
import atlantis.gui.AGUI;
import atlantis.output.ALogInterface;
import atlantis.output.AOutput;
import atlantis.parameters.APar;
import atlantis.utils.A4Vector;
import atlantis.utils.ALogger;
import atlantis.utils.AMath;
import atlantis.utils.AUtilities;

public class AListManager extends JTree implements MouseListener, MouseMotionListener
{
    private static ALogger logger = ALogger.getLogger(AListManager.class);
    private static AEventManager eventManager = AEventManager.instance();

    // used to display the tree
    static private JFrame dialog; // rather than JDialog
    // special nodes
    static private final DefaultMutableTreeNode root = new DefaultMutableTreeNode("Lists");
    static private final DefaultMutableTreeNode highlight = new DefaultMutableTreeNode("Highlight");
    static private final DefaultMutableTreeNode lastDrawn = new DefaultMutableTreeNode("Last Drawn");
    static private final DefaultMutableTreeNode others = new DefaultMutableTreeNode("Others");
    static private final DefaultMutableTreeNode invisible = new DefaultMutableTreeNode("Invisible");
    static private final DefaultMutableTreeNode mass = new DefaultMutableTreeNode("Calculation");
    // instance
    static private final AListManager listManager = new AListManager();
    static private DefaultTreeModel dtm;
    static private int numLists = 0;
    // source and status of Drag and Drop
    static private DefaultMutableTreeNode source = null;
    static private boolean draging = false;
    static private HashMapSet nodeColors;
    
    // counters for V0 mass calc
    static private int V0vx=0;
    static private int V0tracks=0;
    static private int V0charge=0;
    
    // private static final Cursor DROP_VALID = DragSource.DefaultMoveDrop;
    private static final Cursor DROP_VALID_COPY = DragSource.DefaultCopyDrop;
    // private static final Cursor DROP_INVALID = DragSource.DefaultMoveNoDrop;
    
    
    
    private AListManager()
    {
        super(root, true);
        dtm = (DefaultTreeModel) getModel();
        nodeColors = new HashMapSet();
        reset();
        
        addMouseListener(this);
        addMouseMotionListener(this);
        setSelectionRow(1);
        setPreferredSize(new Dimension(200, 300));
        setRootVisible(true);
        setScrollsOnExpand(true);
        setShowsRootHandles(false);
        setCellRenderer(new ColorRenderer());
        
        if (!AGlobals.isAtlantisHeadless()) {
            // set up the visual representation
            dialog = new JFrame("List manager");
            AIcon.setIconImage(dialog);
            Box b = Box.createHorizontalBox();
            b.add(new JScrollPane(this));
            b.add(new ColorPanel());
            dialog.getContentPane().add(b);
            dialog.pack();
            dialog.setDefaultCloseOperation(JFrame.HIDE_ON_CLOSE);
        }
        
    } // AListManager() -----------------------------------------------------
    
    
    
    public static AListManager getInstance()
    {
        return listManager;
        
    } // getInstance() ------------------------------------------------------
    
    
    
    public void showLists()
    {
    	Component gui = AGlobals.instance().getGuiFrame();
        if (gui != null)
        {
            dialog.setLocation(gui.getLocation());
        }
        dialog.setVisible(true);
        
    } // showLists() --------------------------------------------------------
    
    
    
    private boolean isAlreadyInTheList(AList list,
                                       DefaultMutableTreeNode destination)
    {
        boolean already = false;
        for(int i = 0; i < destination.getChildCount(); i++)
        {
            DefaultMutableTreeNode child =
            (DefaultMutableTreeNode) destination.getChildAt(i);
            if(!child.getAllowsChildren())
            {
                AList a = (AList) child.getUserObject();
                if(a.getSource() == list.getSource())
                {
                    int[] items = a.getItems();
                    int newItem = list.getItems()[0];
                    for(int j = 0; j < items.length; ++j)
                    {
                        if(items[j] == newItem)
                        {
                            already = true;
                        }
                    }
                }
            }
        }
        
        return already;
        
    } // isAlreadyInTheList() -----------------------------------------------
    
    
    
    public void add(AList list)
    {
        if(!dialog.isVisible())
        {
            dialog.setVisible(true);
        }
        
        // default destination
        DefaultMutableTreeNode destination = root;
        TreePath tp = getSelectionPath();
        if(tp != null)
        {
            destination = (DefaultMutableTreeNode) tp.getLastPathComponent();
        }
        
        if(destination != null && destination.getAllowsChildren())
        {
            if(destination == others || destination == lastDrawn ||
               destination == highlight || destination == invisible ||
               destination == mass)
            {
                AOutput.alwaysAppend("Can't add to " + destination, ALogInterface.WARNING);
                return;
            }
            
            // check if it's already in the list and if so, don't add
            boolean already = this.isAlreadyInTheList(list, destination);
            if(already)
            {
                logger.info(list + " is already in this list, not added");
            }
            else
            {
                dtm.insertNodeInto(new DefaultMutableTreeNode(list, false),
                                   destination, destination.getChildCount());
                expandPath(new TreePath(dtm.getPathToRoot(destination)));
                ACanvas.getCanvas().repaintAllFromScratch();
            }
        }
        
    } // add() -------------------------------------------------------------
    
    
    
    /**
     * Adds the selected item to the mass list and outputs the invariant mass in the output window
     * Doesn't make the list visible as really only need to see output window
     * @param list
     */
    public void massCalc(AList list)
    {
        // default destination
        DefaultMutableTreeNode destination = mass;
        TreePath tp = getSelectionPath();
        
        if(destination != null && destination.getAllowsChildren())
        {
            // check if it's already in the list and if so, don't add
            boolean already = this.isAlreadyInTheList(list, destination);
            if(already)
            {
                logger.info(list + " is already in this list, not added");
            }
            else
            {
                dtm.insertNodeInto(new DefaultMutableTreeNode(list, false),
                                   destination, destination.getChildCount());
                expandPath(new TreePath(dtm.getPathToRoot(destination)));
                ACanvas.getCanvas().repaintAllFromScratch();
            }
        }
        
        if (tp != null)
        {
            setSelectionPath(tp);
        }
        
        // added to list, now do summarise
        Summarizer summ = new Summarizer();
        AOutput.alwaysAppend(summ.getHeader(), ALogInterface.NORMAL);
        
        AList[] children = getChildren(mass);
        
        int chiLength = children.length;
        
        for(int i = 0; i < chiLength; i++)
        {
            int num = children[i].getItems().length;
            A4Vector v = children[i].getSource().get4Vector(num,
                                                            children[i].getItems(),0.14); //assume pion mass
            
            
            if(v != null && num > 0)
            {
                String info = summ.addAndGetInfo(v, children[i].toString());
                AOutput.alwaysAppend(info, ALogInterface.NORMAL);
            }
        }
        
        String msg = summ.getTotalInfo();
        
        AOutput.alwaysAppend(msg, ALogInterface.NORMAL_BOLD); // Print default summary (ie, assuming pi mass for all)
        
        summ = null;
        
    } // massCalc() -------------------------------------------------------------
    
    /**
     * Adds the selected item to the mass list and outputs the invariant mass in the output window
     * Doesn't make the list visible as really only need to see output window
     * This is the massCalc function for V0 decays. Check for up to 2 tracks and display 4 mass scenarios
     * @param list
     */
    public void massCalcV0(AList list)
    {
    	// default destination
        DefaultMutableTreeNode destination = mass;
        TreePath tp = getSelectionPath();
        
        if(getChildren(mass).length == 0) // if list is empty, make sure counters are reinitialised
        {
        	V0vx=0;
        	V0tracks=0;
        	V0charge=0;
        }
        
        if(destination != null && destination.getAllowsChildren())
        {
            // check if it's already in the list and if so, don't add
            boolean already = this.isAlreadyInTheList(list, destination);
            
            // check list for tracks and vertices (to a maximum of 2 tracks and 1 vertex)
            if(getChildren(mass).length < 3)
            {
            	if(already)
            	{
            		logger.info(list + " is already in this list, not added");
            	}
            	else if(!(list.getSource() instanceof ARVxData || list.getSource() instanceof ATrackData))
            	{
            		logger.info("not selected a track or vertex");
            		AOutput.alwaysAppend("\n\n Not selected a track or vertex\n\n",ALogInterface.PICK);
            		return;
            	}
            	else if((list.getSource() instanceof ARVxData && V0vx>=1) || (list.getSource() instanceof ATrackData && V0tracks>=2))
            	{
            		logger.info("already a vertex or two tracks selected");
            		AOutput.alwaysAppend("\n\n Already a vertex or two tracks selected\n",ALogInterface.PICK);
            		return;
            	}
            	else
            	{
            		
            		// check the charges of the two tracks are opposite
            		//
            		if((list.getSource() instanceof ATrackData))
            		{
            			ATrackData track = (ATrackData) list.getSource();
            			AHelix h = track.getModifiableHelix(list.getItemsID()[0]);
            			if((V0charge != 0) && (V0charge * AMath.getSign(h.pT()) > 0))
            			{
            				logger.info("2nd track is same charge as 1st track");
            				AOutput.alwaysAppend("\n\nPlease pick an oppositely charged track\n", ALogInterface.PICK);
            				return;
            			}
            		} //end check charge
            		
            		// now increment the counters
            		// all tests passed, now can input into list.
            		
            		if(list.getSource() instanceof ATrackData)
            		{
            			V0tracks++;
            			logger.info("Selected a Track");
            		}
            		else if(list.getSource() instanceof ARVxData)
            		{
            			V0vx++;
            			logger.info("Selected a Vertex");
            		}
            		
            		dtm.insertNodeInto(new DefaultMutableTreeNode(list, false),
                                       destination, destination.getChildCount());
            		expandPath(new TreePath(dtm.getPathToRoot(destination)));
            		ACanvas.getCanvas().repaintAllFromScratch();
            	}
                
            }
            else
            {
            	logger.info("list full (max 2 tracks + 1 vertex)");
        		AOutput.alwaysAppend("\n\n List full (max 2 tracks + 1 vertex)\n",ALogInterface.PICK);
            	return;
            }
        }
        if (tp != null)
        {
            setSelectionPath(tp);
        }
        
        
        // added to list, now do summarise
        Summarizer summK = new Summarizer(); // K -> pi pi
        Summarizer summL = new Summarizer(); // Lambda -> pi p
        Summarizer summAL = new Summarizer(); // AntiLambda -> pi pbar
        Summarizer summG = new Summarizer(); // gamma -> ee
        
        double[] secVx = new double[3]; // initialisation of secVx x,y,z array, if no vertex selected, use primary
        double[] priVx = new double[3]; // primary vertex container (used to calculate decay length)
        APar parameterStore = APar.instance();
        priVx[0]=parameterStore.get("Event", "XVtx").getD();
        priVx[1]=parameterStore.get("Event", "YVtx").getD();
        priVx[2]=parameterStore.get("Event", "ZVtx").getD();
        
        secVx=priVx;
        
        AList[] children = getChildren(mass);
        
        
        int chiLength = children.length;
        
    	if(chiLength == 3)
        {
            boolean V0title = true;
            for(int i = 0; i < chiLength; i++)
            {
                AData source = children[i].getSource();
                //ordering of the list is important.
                //if vertex selected second, then the tracks selected before use primary vertex...
                //need to get secVx first, THEN calculate tracks...
                if(source instanceof ARVxData) 
                {	
                    ARVxData rvx = (ARVxData) source;
                    secVx = rvx.getVertex(children[i].getItemsID()[0]); // gets x,y,z of the picked vertex.	
                }
            }
            
            for(int i = 0; i < chiLength; i++)
            {
                
                AData source = children[i].getSource();
                
                if(source instanceof ATrackData)
                {
                    ATrackData track = (ATrackData) source;
                    
                    // get helix of picked track
                    AHelix h = track.getModifiableHelix(children[i].getItemsID()[0]); 
                    
                    // get charge for charge check
                    V0charge = (int) (Math.abs(h.pT())/h.pT());
                    
                    int num = children[i].getItems().length;
                    
                    // initialise 4 vectors
                    A4Vector vK = source.get4Vector(num,
                                                    children[i].getItems(),0.0);
                    A4Vector vL = source.get4Vector(num,
                                                    children[i].getItems(),0.0);
                    A4Vector vAL = source.get4Vector(num,
                                                     children[i].getItems(),0.0);
                    A4Vector vG = source.get4Vector(num,
                                                    children[i].getItems(),0.0);
                    
                    
                    // now calculate the 4 vector
                    if(V0vx > 0)
                    {
            			double R = parameterStore.get("Event", "Curvature").getD() * Math.abs(h.pT());
                        
            			// +1/-1 for a clockwise/anti-clockwise helix 
            			double S = (int) (Math.abs(h.pT())/h.pT());
                        
            			// Coordinates of the center point for the helix
            			double xC = (S * h.d0() - R) * Math.cos(Math.toRadians(h.phi0()) + S * Math.PI / 2.);
            			double yC = (S * h.d0() - R) * Math.sin(Math.toRadians(h.phi0()) + S * Math.PI / 2.);
            			
            			// calculate new phi from centre of curvature and secondary vertex
            			double phiPrime = Math.atan2(yC-secVx[1], xC-secVx[0]) + S*(Math.PI / 2.0);
            			
            			double px = Math.abs(h.pT()) * Math.cos(h.phi0());
            			double py = Math.abs(h.pT()) * Math.sin(h.phi0());
            			double pz = Math.abs(h.pT()) * Math.sinh(h.eta()); 
            			
            			// recalculate px/py at secondary vertex
            			double pxPrime = Math.sqrt((px*px) + (py*py))*Math.cos(phiPrime);
            			double pyPrime = Math.sqrt((px*px) + (py*py))*Math.sin(phiPrime);
            			
            			px = pxPrime;
            			py = pyPrime;
            			
            			if(chiLength == 3 && !parameterStore.get("Minerva", "hideV0massoutput").getStatus())
            			{
                            logger.info("recalculating tracks");
                            AOutput.alwaysAppend("\n\nRecalculated track values:\n", ALogInterface.PICK); 
                            AOutput.alwaysAppend(source.getNameScreenName()+" index: " + (children[i].getItemsID()[0]) + "\n"
                                                 +"PT="+String.format("%.3f",h.pT()) +" GeV\n"
                                                 +AMath.ETA+" = "+String.format("%.3f",h.eta())+"\n"
                                                 +AMath.PHI+" = "+String.format("%.3f",Math.toDegrees(phiPrime))+AMath.DEGREES+"\n"
                                                 +"Px=" + String.format("%.3f",px) +" GeV\n"
                                                 +"Py=" + String.format("%.3f",py) +" GeV\n"
                                                 +"Pz=" + String.format("%.3f",pz) +" GeV\n"
                                                 +"Charge = " + V0charge//Math.abs(V0charge)
                                                 ,ALogInterface.PICK);
            			}
            			else if(chiLength == 3)
            			{
            				if(V0title)
            				{
            					V0title = false;
                                
            					double decayLength = Math.sqrt(Math.pow(secVx[0]-priVx[0], 2.0)+Math.pow(secVx[1]-priVx[1], 2.0)+Math.pow(secVx[2]-priVx[2], 2.0));
            					
            					AOutput.alwaysAppend("\n Decay length for selected vertex: "+String.format("%.2f",decayLength)+" cm\n\n", ALogInterface.PICK);
            					
            					
            					AOutput.alwaysAppend("\n Momentum for selected tracks: \n\n", ALogInterface.PICK);
            				}
            				if(V0charge>0)
            					AOutput.alwaysAppend("+ve track (px,py,pz): ("+String.format("%.3f",px)+", "+String.format("%.3f",py)+", "+String.format("%.3f",pz)+")\n", ALogInterface.PICK);
            				else if(V0charge <0)
            					AOutput.alwaysAppend("-ve track (px,py,pz): ("+String.format("%.3f",px)+", "+String.format("%.3f",py)+", "+String.format("%.3f",pz)+")\n", ALogInterface.PICK);
            				
            			}
            			
            			
            			/* now check charges of particles again and add to corresponding 4vectors
                         *
                         *	scenarios:
                         *
                         *	(K) Kaon -> pi+ pi-
                         *	(L) Lambda -> proton pi-
                         *	(AL)AntiLambda -> antiproton pi+
                         *	(G) Gamma -> e+ e-
                         *
                         *	
                         *
                         */
            			
            			// negative charge
            			if(V0charge < 0)
            			{
            				vK.set(px, py, pz, 0.1396);
            				vL.set(px, py, pz, 0.1396);
            				vAL.set(px, py, pz, 0.938);
            				vG.set(px, py, pz, 0.000511);
            			}
            			// positive charge
            			else if(V0charge > 0)
            			{
            				vK.set(px, py, pz, 0.1396);
            				vL.set(px, py, pz, 0.938);
            				vAL.set(px, py, pz, 0.1396);
            				vG.set(px, py, pz, 0.000511);
            			}
            			
            			
                        
                        // this is to display each track summary. 
                        // currently off as too much summary crowds the output
                        if(vK != null && vL != null && vAL != null && vG != null && num > 0 && chiLength == 3)
                        {
                            String info = summK.addAndGetInfo(vK, children[i].toString());
                            //AOutput.alwaysAppend(info, ALogPane.NORMAL);
                            info = summL.addAndGetInfo(vL, children[i].toString());
                            //AOutput.alwaysAppend(info, ALogPane.NORMAL);
                            info = summAL.addAndGetInfo(vAL, children[i].toString());
                            //AOutput.alwaysAppend(info, ALogPane.NORMAL);
                            info = summG.addAndGetInfo(vG, children[i].toString());
                            //AOutput.alwaysAppend(info, ALogPane.NORMAL);
                        }
                    }
                    
                }
            }
            
            if(chiLength ==3 && !parameterStore.get("Minerva", "hideV0massoutput").getStatus())
            {
                // Config switch to display either mass or px,py,pz.
                String msgK = summK.getTotalInfo();
                String msgL = summL.getTotalInfo();
                String msgAL = summAL.getTotalInfo();
                String msgG = summG.getTotalInfo();
                
                AOutput.alwaysAppend("\n V0 Decay Scenarios", ALogInterface.NORMAL_BOLD);
                
                AOutput.alwaysAppend(summK.getHeader(), ALogInterface.NORMAL);
                
                // print out the resultant mass calculation for each scenario
                AOutput.alwaysAppend("K"+AMath.RARROW+AMath.PION+AMath.SUPPLUS+AMath.PION+AMath.SUPMINUS+"\n", ALogInterface.NORMAL); 
                AOutput.alwaysAppend(msgK, ALogInterface.NORMAL_BOLD); // Print kaon summary
                AOutput.alwaysAppend("\n"+AMath.LAMBDACAP+AMath.RARROW+"p"+AMath.PION+AMath.SUPMINUS+"\n", ALogInterface.NORMAL); 
                AOutput.alwaysAppend(msgL, ALogInterface.NORMAL_BOLD); // Print lambda summary
                AOutput.alwaysAppend("\n"+AMath.LAMBDACAP+AMath.OVERLINE+AMath.RARROW+"p"+AMath.OVERLINE+AMath.PION+AMath.SUPPLUS+"\n", ALogInterface.NORMAL); 
                AOutput.alwaysAppend(msgAL, ALogInterface.NORMAL_BOLD); // Print antilambda summary
                AOutput.alwaysAppend("\n"+AMath.GAMMA+AMath.RARROW+"e"+AMath.SUPPLUS+"e"+AMath.SUPMINUS+"\n", ALogInterface.NORMAL); 
                AOutput.alwaysAppend(msgG, ALogInterface.NORMAL_BOLD); // Print gamma summary
                AOutput.alwaysAppend("\n", ALogInterface.NORMAL);
            }
        }
        summK = null;
        summL = null;
        summAL = null;
        summG = null;
        
    } // massCalcV0() -------------------------------------------------------------
    
    /**
     * Takes two vectors from a list (added with the pick interaction and P key modifier)
     * and prints the delta R and delta Phi of the vectors
     * @param list
     */
    public void deltaR(AList list)
    {
    	// default destination
        DefaultMutableTreeNode destination = mass;
        TreePath tp = getSelectionPath();
        
        if(destination != null && destination.getAllowsChildren())
        {
            // check if it's already in the list and if so, don't add
            boolean already = this.isAlreadyInTheList(list, destination);
            
            if(already)
            {
                logger.info(list + " is already in this list, not added");
            }
            else
            {
                if(getChildren(mass).length >= 2){
                    logger.info(list + " is full, maximum of 2 objects for delta phi calculation " +
                                "\n Clearing list");
                    //AOutput.alwaysAppend("\n\n List full \n Clearing List \n",ALogPane.PICK);
                    mass.removeAllChildren();
                }
                dtm.insertNodeInto(new DefaultMutableTreeNode(list, false),
                                   destination, destination.getChildCount());
                expandPath(new TreePath(dtm.getPathToRoot(destination)));
                ACanvas.getCanvas().repaintAllFromScratch();
            }
        }
        
        if (tp != null)
        {
            setSelectionPath(tp);
        }
        
        AList[] children = getChildren(mass);
        
        int chiLength = children.length;
        if(chiLength == 2){
        	A4Vector v1 = children[0].getSource().get4Vector(children[0].getItems().length,
                                                             children[0].getItems(),0.0);
        	A4Vector v2 = children[1].getSource().get4Vector(children[1].getItems().length,
                                                             children[1].getItems(),0.0);
            
            if(v1 != null && v2 != null)
            {
                double deltaPhi = Math.abs(v1.getPhi() - v2.getPhi()) > Math.PI ? (2*Math.PI) - Math.abs(v1.getPhi() - v2.getPhi()) : Math.abs(v1.getPhi() - v2.getPhi());
                double deltaEta = Math.abs(v1.getEta() - v2.getEta());
                double deltaR = Math.sqrt(deltaPhi*deltaPhi + deltaEta*deltaEta);
                
                String info = "\n\n"+AMath.DELTA+AMath.PHI+" = "+String.format("%.1f",Math.toDegrees(deltaPhi))+AMath.DEGREES+" ("+String.format("%.3f",deltaPhi)+")"
                + "\n"+AMath.DELTA+"R = "+String.format("%.3f",deltaR)+"\n";
                AOutput.alwaysAppend(info, ALogInterface.NORMAL_BOLD);
            }
            
        }
    }
    
    public void highlight(AList list)
    {
        highlight.removeAllChildren();
        dtm.insertNodeInto(new DefaultMutableTreeNode(list, false), highlight, 0);
        dtm.reload(highlight);
        expandPath(new TreePath(dtm.getPathToRoot(highlight)));
        ACanvas.getCanvas().repaintAllFromScratch();
        
    } // highlight() --------------------------------------------------------
    
    
    
    public void addLastDrawn(AList list)
    {
        dtm.insertNodeInto(new DefaultMutableTreeNode(list, false), lastDrawn, 0);
        dtm.reload(lastDrawn);
        
    } // addLastDrawn() -----------------------------------------------------
    
    
    
    public void addInvisible(AList list)
    {
        if(!dialog.isVisible())
        {
            dialog.setVisible(true);
        }
        
        // check if it's already in the list and if so, don't add
        boolean already = this.isAlreadyInTheList(list, invisible);
        if(already)
        {
            logger.info(list + " is already in the Invisible list, not added");
        }
        else
        {
            dtm.insertNodeInto(new DefaultMutableTreeNode(list, false),
                               invisible, invisible.getChildCount());
            expandPath(new TreePath(dtm.getPathToRoot(invisible)));
            ACanvas.getCanvas().repaintAllFromScratch();
        }
        
    } // addInvisible() -----------------------------------------------------
    
    
    /**
     * When adding items to the list using rubberband they are grouped by type
     * and appear as "InDetTrack (5 items)". This method allows the user to expand
     * these items so they can manipulate individual items.
     * @param s Collection
     * @return Action
     */
    private Action expand(final Collection s)
    {
        // This is called when the user right-clicks, we first determine if this 
        // item can be expanded or not.
        if (s.contains(root) || s.size() != 1)
        {
            return null;
        }
        
        DefaultMutableTreeNode node = (DefaultMutableTreeNode) s.iterator().next();
        if (node.getUserObject() instanceof AList)
        {
            AList a = (AList) node.getUserObject();
            if (a.getItems().length < 1)
            {
                return null;
            }
        }
        else
        {
            return null;
        }
        
        // It can be expanded, so we add a menu item to the popup with the callback 
        // function as specified below.
        return new AbstractAction("Expand items")
        {
            public void actionPerformed(ActionEvent e)
            {
                // We checked these casts before adding the callback
                DefaultMutableTreeNode node = (DefaultMutableTreeNode) s.iterator().next();
                AList a = (AList) node.getUserObject();
                DefaultMutableTreeNode parent = (DefaultMutableTreeNode) node.getParent();
                int index = parent.getIndex(node);
                
                // Now go through the list and add the items one by one to the parent.
                for (int i=0; i<a.getItems().length; i++)
                {
                    AList list = new AList(a.getSource(), a.getItems()[i]);
                    dtm.insertNodeInto(new DefaultMutableTreeNode(list, false), parent, index);
                }
                
                // And remove the old list.
                dtm.removeNodeFromParent(node);
            }
        };
        
    } // expand() -----------------------------------------------------------
    
    
    private Action remove(final Collection s)
    {
        if(s.contains(root))
        {
            return null;
        }
        
        return new AbstractAction("Remove")
        {
            public void actionPerformed(ActionEvent e)
            {
                for (Iterator i = s.iterator(); i.hasNext();)
                {
                    DefaultMutableTreeNode node = (DefaultMutableTreeNode) i.next();
                    if (node.getUserObject() instanceof AList)
                    {
                        AList a = (AList) node.getUserObject();
                        a.getSource().remove(a.getItems());
                    }
                    dtm.removeNodeFromParent(node);
                    ACanvas.getCanvas().repaintAllFromScratch();
                }
            }
        };
        
    } // remove() -----------------------------------------------------------
    
    
    
    /**
     * This method is called after right-click -> summarize on the list-item in
     * the list dialog
     * @param s Collection
     * @return Action
     */
    private Action newSummarize(final Collection s)
    {
        if(s.contains(root) || s.size() != 1)
        {
            return null;
        }
        
        return new AbstractAction("Summarize")
        {
            public void actionPerformed(ActionEvent e)
            {
                Summarizer summ = new Summarizer();
                AOutput.alwaysAppend(summ.getHeader(), ALogInterface.NORMAL);
                
                for(Iterator j = s.iterator(); j.hasNext();)
                {
                    DefaultMutableTreeNode node = (DefaultMutableTreeNode) j.next();
                    AList[] children = getChildren(node);
                    
                    for(int i = 0; i < children.length; i++)
                    {
                        int num = children[i].getItems().length;
                        AList child = children[i];
                        AData source = child.getSource();
                        A4Vector v = source.get4Vector(num, child.getItems());
                        
                        if(v != null && num > 0)
                        {
                            String info = summ.addAndGetInfo(v, children[i].toString());
                            AOutput.alwaysAppend(info, ALogInterface.NORMAL);
                        }
                    }
                }
                
                String msg = summ.getTotalInfo();
                AOutput.alwaysAppend(msg, ALogInterface.NORMAL_BOLD);
                
                summ = null;
                
            } // actionPerformed()
        };
        
    } // newSummarize() -----------------------------------------------------
    
    
    
    /**
     * This summarize method is called from rubberbanding menu - Summarize
     * (from ASelection class)
     */
    public void summarize()
    {
        if(lastDrawn == null)
        {
            return;
        }
        
        Summarizer summ = new Summarizer();
        
        AList[] children = getChildren(lastDrawn);
        
        AOutput.alwaysAppend(summ.getHeader(), ALogInterface.NORMAL);
        
        for(int i = 0; i < children.length; i++)
        {
            int num = children[i].getItems().length;
            A4Vector v = children[i].getSource().get4Vector(num, children[i].getItems());
            
            if(v != null && num > 0)
            {
                String msg = summ.addAndGetInfo(v, children[i].toString());
                AOutput.alwaysAppend(msg, ALogInterface.NORMAL);
            }
        }
        
        String msg = summ.getTotalInfo();
        AOutput.alwaysAppend(msg, ALogInterface.NORMAL_BOLD);
        
        summ = null;
        
    } // summarize() --------------------------------------------------------
    
    
    
    private Action reset(final Collection s)
    {
        if(s.size() == 1 && s.contains(root))
        {
            return new AbstractAction("Reset")
            {
                public void actionPerformed(ActionEvent e)
                {
                    reset();
                    ACanvas.getCanvas().repaintAllFromScratch();
                }
            };
        }
        else
        {
            return null;
        }
        
    } // reset() ------------------------------------------------------------
    
    
    
    public void clearHighlight()
    {
        highlight.removeAllChildren();
        dtm.reload(highlight);
        ACanvas.getCanvas().repaintAllFromScratch();
        
    } // clearHighlight() ---------------------------------------------------
    
    
    
    public void clearHighlightAndMassCalculation()
    {
        highlight.removeAllChildren();
        dtm.reload(highlight);
        mass.removeAllChildren();
        dtm.reload(mass);
        ACanvas.getCanvas().repaintAllFromScratch();
        
    } // clearHighlightAndMassCalculation() ---------------------------------------------------
    
    
    
    public void clearLastDrawn()
    {
        lastDrawn.removeAllChildren();
        dtm.reload(lastDrawn);
        
    } // clearLastDrawn() ---------------------------------------------------
    
    
    
    /**
     * Check if list (list of IDs id) of datatimes (toMask datatype) is present
     * in the current event and if so, the items are added into the Invisible
     * list, thus masked and igrored.
     *
     * @param toMask AData
     * @param id int[]
     * @param index int[]
     */
    private boolean checkAndAddIntoInvisible(int[] id, int[] indexCheck, String key)
    {
        boolean result = false;
        
        // check if the datatype (key) exists in the new event
        AEvent e = eventManager.getCurrentEvent();
        AData toMask = e.get(key);
        
        if(toMask != null)
        {
            // iterate over all data items in this list
            for(int i = 0; i < id.length; i++)
            {
                // check if this ID is present in the current event
                int index = toMask.getIndexFromId(id[i]);
                if(index != -1)
                {
                    // AList expects datatype and its index
                    AList l = new AList(toMask, index);
                    addInvisible(l);
                    AOutput.alwaysAppend("  " + l.toString() + "\n", ALogInterface.NORMAL);
                    result = true;
                    
                    // 2007-08-29: debugging check, remove after some time if the
                    // problem is not observed (then indexCheck param is not needed)
                    if(index != indexCheck[i])
                    {
                        logger.warn("AListManager: problem, indices not equal!");
                    }
                    // </debug>
                }
            }
        }
        
        return result;
        
    } // checkAndAddIntoInvisible() -----------------------------------------
    
    
    
    /**
     * restore Invisible list, cross-event invisible
     * @param backup AList[]
     */
    private void restoreInvisible(AList[] backup)
    {
        if(backup.length > 0)
        {
            AOutput.alwaysAppend("\nPrevious Invisible list not empty, masked items:\n",
                                 ALogInterface.WARNING);
            
            boolean somethingInvisible = false;
            
            for(int i = 0; i < backup.length; i++)
            {
                AData d = backup[i].getSource();
                String sg = d.getStoreGateKey();
                String accessKey = d.getName() + (sg != null ? sg : "");
                
                if(backup[i].getItems().length > 1)
                {
                    somethingInvisible = checkAndAddIntoInvisible(backup[i].getItemsID(),
                                                                  backup[i].getItems(), accessKey);
                }
                else
                {
                    int id = backup[i].getItemsID()[0];
                    int index = backup[i].getItems()[0];
                    somethingInvisible = checkAndAddIntoInvisible(new int[] { id },
                                                                  new int[] { index }, accessKey);
                }
                
            } // for (overall items in the Invisible list)
            
            if(!somethingInvisible)
            {
                AOutput.alwaysAppend("  none\n", ALogInterface.NORMAL);
            }
            
        } // if(backup.length > 0)
        
    } // restoreInvisible() -------------------------------------------------
    
    
    
    public void resetAndPreserveInvisible()
    {
        AList[] invisibleBackup = getChildren(invisible);
        reset();
        restoreInvisible(invisibleBackup);
        
    } // resetAndPreserveInvisible() ----------------------------------------
    
    
    
    public void reset()
    {
        root.removeAllChildren();
        highlight.removeAllChildren();
        lastDrawn.removeAllChildren();
        invisible.removeAllChildren();
        mass.removeAllChildren();
        
        root.add(highlight);
        root.add(lastDrawn);
        root.add(others);
        root.add(invisible);
        root.add(mass);
        
        numLists = 0;
        root.add(new DefaultMutableTreeNode("List " + numLists++));
        
        nodeColors.clear();
        nodeColors.put(highlight, new Integer(AColorMap.WH));
        nodeColors.put(invisible, new Integer(AColorMap.INVISIBLE));
        nodeColors.put(mass, new Integer(AColorMap.WH));
        dtm.reload();
        
    } // reset() ------------------------------------------------------------
    
    
    
    private Action rename(Collection s)
    {
        if(s.size() != 1)
        {
            return null;
        }
        
        final DefaultMutableTreeNode selected =
        (DefaultMutableTreeNode) (s.iterator().next());
        
        if(!selected.getAllowsChildren() || selected == root ||
           selected == others || selected == highlight ||
           selected == lastDrawn || selected == invisible ||
           selected == mass)
        {
            return null;
        }
        
        return new AbstractAction("Rename")
        {
            public void actionPerformed(ActionEvent h)
            {
                String name = JOptionPane.showInputDialog(listManager,
                                                          "Enter new name");
                selected.setUserObject(name);
            }
        };
        
    } // rename() -----------------------------------------------------------
    
    
    
    private Action newParent(final Collection s)
    {
        if(s.contains(root))
        {
            return null;
        }
        
        Set parents = new HashSet();
        
        for(Iterator i = s.iterator(); i.hasNext();)
        {
            parents.add(((TreeNode) (i.next())).getParent());
        }
        
        if(parents.size() != 1)
        {
            return null;
        }
        
        final DefaultMutableTreeNode oldParent =
        (DefaultMutableTreeNode) (parents.iterator().next());
        
        return new AbstractAction("New Parent")
        {
            public void actionPerformed(ActionEvent e)
            {
                DefaultMutableTreeNode newParent =
                new DefaultMutableTreeNode("List " + numLists++);
                int index = 0;
                for(Iterator i = s.iterator(); i.hasNext();)
                {
                    index = Math.min(oldParent.getIndex((TreeNode) (i.next())),
                                     index);
                }
                
                for(Iterator i = s.iterator(); i.hasNext();)
                {
                    dtm.removeNodeFromParent((MutableTreeNode) (i.next()));
                }
                
                dtm.insertNodeInto(newParent, oldParent, index);
                
                for(Iterator i = s.iterator(); i.hasNext();)
                {
                    dtm.insertNodeInto((MutableTreeNode) (i.next()), newParent, 0);
                }
                
                expandPath(new TreePath(dtm.getPathToRoot(newParent)));
            }
        };
        
    } // newParent() --------------------------------------------------------
    
    
    
    private Action newChild(final Collection s)
    {
        if(s.size() != 1)
        {
            return null;
        }
        
        final DefaultMutableTreeNode selected =
        (DefaultMutableTreeNode) (s.iterator().next());
        
        if(!selected.getAllowsChildren())
        {
            return null;
        }
        
        return new AbstractAction("New Child")
        {
            public void actionPerformed(ActionEvent h)
            {
                dtm.insertNodeInto(new DefaultMutableTreeNode("List " + numLists++),
                                   selected, 0);
                expandPath(new TreePath(dtm.getPathToRoot(selected)));
            }
        };
        
    } // newChild() ---------------------------------------------------------
    
    
    
    private DefaultMutableTreeNode getNode(Point m)
    {
        TreePath p = getPathForLocation(m.x, m.y);
        if(p != null)
        {
            TreeNode tn = (TreeNode) p.getLastPathComponent();
            if(tn instanceof DefaultMutableTreeNode)
            {
                return (DefaultMutableTreeNode) tn;
            }
        }
        return null;
        
    } // getNode() ----------------------------------------------------------
    
    
    
    private static class HashMapSet extends HashMap
    {
        public Object put(Object key, Object value)
        {
            // don't allow duplicates
            if(containsKey(key))
            {
                this.remove(key);
            }
            
            if(value != null)
            {
                return super.put(key, value);
            }
            
            else
            {
                return null;
            }
        }
    }
    
    
    
    class ColorRenderer extends DefaultTreeCellRenderer
    {
        AColorIcon folderIcon;
        AColorIcon leafIcon;
        
        public ColorRenderer()
        {
            folderIcon = new AColorIcon(AColorMap.WH, 15, 11);
            leafIcon = new AColorIcon(AColorMap.WH, 11, 11);
        }
        
        public Component getTreeCellRendererComponent(JTree tree, Object value, boolean sel, boolean expanded, boolean leaf, int row, boolean hasFocus)
        {
            
            super.getTreeCellRendererComponent(tree, value, sel, expanded, leaf, row, hasFocus);
            
            int c = getColor((TreeNode) value);
            if (c == AColorMap.NO_COLOR)
                setToolTipText("No Color");
            else if (c == AColorMap.INVISIBLE)
                setToolTipText("Invisible");
            else
                setToolTipText("");
            
            if (leaf)
            {
                leafIcon.setColorIndex(c);
                setIcon(leafIcon);
            }
            else
            {
                folderIcon.setColorIndex(c);
                setIcon(folderIcon);
            }
            
            return this;
        }
        
    }
    
    private int getColor(TreeNode node)
    {
        TreeNode[] parents = dtm.getPathToRoot(node);
        int color = AColorMap.NO_COLOR;
        for (int i = 0; i < parents.length; ++i)
        {
            Object o = nodeColors.get(parents[i]);
            if (o != null)
                color = ((Integer) o).intValue();
        }
        return color;
    }
    
    private DefaultMutableTreeNode[] getLeafs()
    {
        java.util.List list = new ArrayList();
        for (int i = 0; i < root.getChildCount(); ++i)
        {
            DefaultMutableTreeNode node = (DefaultMutableTreeNode) root.getChildAt(i);
            if (node != lastDrawn)
            {
                if (node.getAllowsChildren())
                {
                    Enumeration descendents = node.breadthFirstEnumeration();
                    while (descendents.hasMoreElements())
                    {
                        DefaultMutableTreeNode descendent = (DefaultMutableTreeNode) (descendents.nextElement());
                        if (!descendent.getAllowsChildren())
                            list.add(descendent);
                    }
                }
                else
                {
                    list.add(node);
                }
            }
        }
        return (DefaultMutableTreeNode[]) list.toArray(new DefaultMutableTreeNode[list.size()]);
    }
    
    public int[][] getColorMapping(AData data)
    {
        DefaultMutableTreeNode[] leafs = getLeafs();
        int num = 0;
        for (int i = 0; i < leafs.length; ++i)
            if (((AList) (leafs[i].getUserObject())).getSource() == data)
                num += ((AList) (leafs[i].getUserObject())).getItems().length;
        
        int[] index = new int[num];
        int[] color = new int[num];
        num = 0;
        for (int i = 0; i < leafs.length; ++i)
        {
            if (((AList) (leafs[i].getUserObject())).getSource() == data)
            {
                int[] items = ((AList) (leafs[i].getUserObject())).getItems();
                int c = getColor(leafs[i]);
                for (int j = 0; j < items.length; ++j)
                {
                    index[num] = items[j];
                    color[num] = c;
                    num++;
                }
            }
        }
        int[][] temp = new int[2][];
        temp[0] = index;
        temp[1] = color;
        return temp;
    }
    
    public int getColorOfOthers()
    {
        return getColor(others);
    }
    
    public boolean[] getSelection(AData data)
    {
        boolean[] selected = new boolean[data.getNumData()];
        
        TreePath[] paths = getSelectionPaths();
        if (paths != null)
        {
            for (int i = 0; i < paths.length; ++i)
            {
                Enumeration nodes = ((DefaultMutableTreeNode) (paths[i].getLastPathComponent())).breadthFirstEnumeration();
                while (nodes.hasMoreElements())
                {
                    DefaultMutableTreeNode node = (DefaultMutableTreeNode) (nodes.nextElement());
                    if (!node.getAllowsChildren())
                    {
                        AList a = (AList) (node.getUserObject());
                        if (a.getSource() == data)
                        {
                            int[] items = a.getItems();
                            for (int j = 0; j < items.length; ++j)
                                selected[items[j]] = true;
                        }
                    }
                }
            }
        }
        return selected;
    }
    
    public AList[] getChildren(DefaultMutableTreeNode node)
    {
        java.util.List children = new ArrayList();
        
        Object o = node.getUserObject();
        if (o instanceof AList)
        {
            children.add(o);
        }
        else
        {
            // process the child nodes recursively
            for (int i = 0; i < node.getChildCount(); ++i)
            {
                AList[] grandChildren = getChildren((DefaultMutableTreeNode) node.getChildAt(i));
                for (int j = 0; j < grandChildren.length; ++j)
                {
                    children.add(grandChildren[j]);
                }
            }
        }
        return (AList[]) (children.toArray(new AList[children.size()]));
    }
    
    public static Collection uniqueExpansion(Collection s)
    {
        Collection c = new HashSet();
        for (Iterator i = s.iterator(); i.hasNext();)
        {
            Enumeration en = ((DefaultMutableTreeNode) (i.next())).depthFirstEnumeration();
            while (en.hasMoreElements())
            {
                c.add(en.nextElement());
            }
        }
        return c;
    }
    
    class ColorPanel extends JPanel
    {
        ColorPanel()
        {
            Color[] colorMap = AColorMap.getColors();
            ActionListener a = new ActionListener()
            {
                public void actionPerformed(ActionEvent e)
                {
                    int colorIndex = ((AColorIcon) (((JButton) (e.getSource())).getIcon())).getColorIndex();
                    TreePath[] sel = getSelectionPaths();
                    if (sel != null)
                    {
                        for (int i = 0; i < sel.length; ++i)
                            nodeColors.put(sel[i].getLastPathComponent(), new Integer(colorIndex));
                    }
                    listManager.repaint();
                    ACanvas.getCanvas().repaintAllFromScratch();
                }
            };
            Box b = Box.createVerticalBox();
            for (int i = 0; i < colorMap.length; i++)
            {
                JButton l = new JButton(new AColorIcon(i, 13, 13));
                l.setMargin(new Insets(0, 0, 0, 0));
                l.setBorder(null);
                b.add(l);
                b.add(Box.createVerticalStrut(3));
                l.addActionListener(a);
            }
            b.add(Box.createVerticalStrut(6));
            JButton l = new JButton(new AColorIcon(AColorMap.NO_COLOR, 13, 13));
            l.setMargin(new Insets(0, 0, 0, 0));
            l.setBorder(null);
            l.setToolTipText("No specified color");
            b.add(l);
            b.add(Box.createVerticalStrut(3));
            l.addActionListener(a);
            l = new JButton(new AColorIcon(AColorMap.INVISIBLE, 13, 13));
            l.setMargin(new Insets(0, 0, 0, 0));
            l.setBorder(null);
            l.setToolTipText("Invisible");
            b.add(l);
            l.addActionListener(a);
            super.add(b);
        }
    }
    
    
    
    public ArrayList getLastDrawn()
    {
        ArrayList result = new ArrayList();
        
        Enumeration myenum = lastDrawn.children();
        while(myenum.hasMoreElements())
        {
            Object o = ((DefaultMutableTreeNode) (myenum.nextElement())).getUserObject();
            if(o instanceof AList)
            {
                if(((AList) o).getItems().length > 0)
                {
                    result.add(o);
                }
            }
        }
        
        return result;
    }
    
    
    
    public void copyLastDrawn()
    {
        
        DefaultMutableTreeNode copy = new DefaultMutableTreeNode("List " + numLists++);
        Enumeration myenum = lastDrawn.children();
        while (myenum.hasMoreElements())
        {
            Object o = ((DefaultMutableTreeNode) (myenum.nextElement())).getUserObject();
            if (o instanceof AList)
            {
                if (((AList) o).getItems().length > 0)
                    copy.add(new DefaultMutableTreeNode(o, false));
            }
        }
        dtm.insertNodeInto(copy, root, 0);
        setSelectionPath(new TreePath(dtm.getPathToRoot(copy)));
        // expandPath(getPathForLocation(e.getX(), e.getY()));
    }
    
    public void mouseExited(MouseEvent e)
    {
        update(e);
    } // DnD
    
    public void mouseClicked(MouseEvent e)
    {}
    
    public void mouseEntered(MouseEvent e)
    {
        update(e);
    } // DnD
    
    
    public void mousePressed(MouseEvent e)
    {
        // source of DnD
        source = getNode(e.getPoint());
        // set picked object to selection if possible
        TreePath tp = getPathForLocation(e.getPoint().x, e.getPoint().y);
        if (tp != null)
        {
        	
            DefaultMutableTreeNode node = (DefaultMutableTreeNode) (tp.getLastPathComponent());
            if (!node.getAllowsChildren())
            {
                AList a = (AList) (node.getUserObject());
                if (a.getItems().length == 1)
                    APickingGraphics2D.setPicked(a.getSource(), a.getItems()[0]);
            }
        }
        // special for right button
        if (!AUtilities.isRightMouseButton(e))
            return;
        // make right button act on selection just as left button does
        if (tp != null)
        {
            if (e.isControlDown())
            {
                addSelectionPath(tp);
            }
            else
            {
                setSelectionPath(tp);
            }
        }
        // pop up menu on right click
        TreePath[] sel = getSelectionPaths();
        Collection selection = new ArrayList(sel.length);
        for (int i = 0; i < sel.length; ++i)
            selection.add(sel[i].getLastPathComponent());
        
        JPopupMenu popup = new JPopupMenu();
        Action temp;
        temp = expand(selection);
        if (temp != null)
            popup.add(temp);
        temp = remove(selection);
        if (temp != null)
            popup.add(temp);
        temp = reset(selection);
        if (temp != null)
            popup.add(temp);
        temp = rename(selection);
        if (temp != null)
            popup.add(temp);
        temp = newParent(selection);
        if (temp != null)
            popup.add(temp);
        temp = newChild(selection);
        if (temp != null)
            popup.add(temp);
        temp = newSummarize(selection);
        if (temp != null)
            popup.add(temp);
        // gary just remove this for now
        
        AListProcessor[] listProcessors = eventManager.getCurrentEvent().getData();
        
        for (int i = 0; i < listProcessors.length; i++)
        {
            Action[] actions = listProcessors[i].getActions(selection);
            for (int j = 0; j < actions.length; ++j)
            {
                popup.add(actions[j]);
            }
        }
        
        popup.show(e.getComponent(), e.getX(), e.getY());
        
    }
    
    public void mouseMoved(MouseEvent e)
    {}
    
    public void mouseDragged(MouseEvent e)
    {
        // start of drag
        if (!draging)
        {
            if (source != null && isDragSource(source))
                draging = true;
        }
        update(e);
    }
    
    public void mouseReleased(MouseEvent e)
    {
        // DnD event
        if (draging && source != null)
        {
            DefaultMutableTreeNode destination = getNode(e.getPoint());
            if (destination != null && source != destination && destination.getAllowsChildren() && isValidDrag(source, destination))
            {
                if (!e.isControlDown() && source != lastDrawn)
                {
                    dtm.removeNodeFromParent(source);
                }
                if (source == lastDrawn)
                {
                    DefaultMutableTreeNode copy = new DefaultMutableTreeNode("List " + numLists++);
                    Enumeration myenum = source.children();
                    while (myenum.hasMoreElements())
                    {
                        Object o = ((DefaultMutableTreeNode) (myenum.nextElement())).getUserObject();
                        copy.add(new DefaultMutableTreeNode(o, false));
                    }
                    dtm.insertNodeInto(copy, destination, 0);
                }
                else
                    dtm.insertNodeInto(source, destination, 0);
                expandPath(getPathForLocation(e.getX(), e.getY()));
                ACanvas.getCanvas().repaintAllFromScratch();
            }
        }
        draging = false;
        source = null;
        setCursor(ACursorFactory.getInstance().getDefaultCursor());
    }
    
    private boolean isDragSource(DefaultMutableTreeNode source)
    {
        if (source == null || source == root || source == highlight || source == others || source==mass)
            return false;
        else
            return true;
    }
    
    private boolean isValidDrag(DefaultMutableTreeNode source, DefaultMutableTreeNode destination)
    {
        if (source == null || destination == null)
            return false;
        if (source == root || source == highlight || source == others || source == mass)
            return false;
        if (destination == others)
            return false;
        if (destination == lastDrawn)
            return false;
        if (destination == highlight)
        {
            if (source.getAllowsChildren())
                return false;
            if (((AList) (source.getUserObject())).getItems().length != 1)
                return false;
            if (highlight.getChildCount() != 0)
                return false;
        }
        return true;
    }
    
    private void update(MouseEvent e)
    {
        if (!draging)
            return;
        Point point = e.getPoint();
        DefaultMutableTreeNode current = getNode(point);
        if (current != null && current.getAllowsChildren() && isValidDrag(source, current))
        {
            TreePath p = getPathForLocation(point.x, point.y);
            setSelectionPath(p);
            if (e.isControlDown())
            {
                setCursor(DROP_VALID_COPY);
            }
            else
            {
                setCursor(ACursorFactory.getInstance().getDragValidCursor());
            }
        }
        else
        {
            setCursor(ACursorFactory.getInstance().getDragInvalidCursor());
        }
    }
    
}


class Summarizer
{
    private A4Vector vector = null;
    private double etSum = 0;
    
    
    public Summarizer()
    {
        vector = new A4Vector();
        
    }
    
    
    public String getHeader()
    {
        StringBuffer sb = new StringBuffer();
        sb.append("\nSummary output:\n");
        sb.append("                  "); // 18 spaces
        sb.append("    PT  ");
        sb.append("    ET  ");
        sb.append("    E   ");
        sb.append("    M   ");
        sb.append("    MT  ");
        sb.append("\n");
        
        return sb.toString();
        
    }
    
    
    public String addAndGetInfo(A4Vector item, String description)
    {
        vector.add(item);
        etSum += item.getEt();
        StringBuffer sb = new StringBuffer();
        sb.append(description);
        sb.append("\n");
        sb.append("                  "); // 18 spaces
        sb.append(String.format("%7.1f",item.getPt()));
        sb.append(String.format("%8.1f",item.getEt()));
        sb.append(String.format("%7.1f",item.getE()));
        sb.append(String.format("%8.3f",item.getMass()));
        sb.append(String.format("%8.1f",item.getMt()));
        sb.append("\n");
        
        return sb.toString();
    }
    
    public double getTotalEt()
    {
    	return etSum;
    }
    
    public double getTotalPt()
    {
    	return vector.getPt();
    }
    
    public double getTotalMt()
    {
    	double et = getTotalEt();
    	double pt = getTotalPt();
    	return Math.sqrt(et*et - pt*pt);
    }
    
    public String getTotalInfo()
    {
    	StringBuffer sb = new StringBuffer();
        sb.append(String.format("%-18s", "Total"));
        sb.append(String.format("%7.1f",getTotalPt()));
        sb.append(String.format("%8.1f",getTotalEt()));
        sb.append(String.format("%7.1f",vector.getE()));
        sb.append(String.format("%8.3f",vector.getMass()));
        sb.append(String.format("%8.1f",getTotalMt()));
        sb.append("\n");
        
        return sb.toString();
        
    }
    
}


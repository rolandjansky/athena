package atlantis.interactions;

import java.awt.Graphics2D;
import java.awt.Point;
import java.awt.Rectangle;
import java.awt.Robot;
import java.awt.AWTException;
import java.awt.geom.Point2D;
import java.awt.event.KeyEvent;
import javax.swing.SwingUtilities;

import atlantis.list.AList;
import atlantis.list.AListManager;
import atlantis.nge.ANManager;
import atlantis.nge.ANProjectionPhysics;
import atlantis.output.ALogInterface;
import atlantis.output.AOutput;
import atlantis.canvas.ACanvas;
import atlantis.canvas.AWindow;
import atlantis.data.ALVL1TriggerTowerData;
import atlantis.data.AMuonHitData;
import atlantis.parameters.APar;
import atlantis.projection.AProjectionNPhysics;
import atlantis.projection.AProjectionTrackResidual;
import atlantis.projection.AProjectionsManager;
import atlantis.event.AData;
import atlantis.data.AJetData;
import atlantis.data.ACalorimeterData;
import atlantis.data.ALArData;
import atlantis.data.ASVxData;
import atlantis.data.ARVxData;
import atlantis.data.ATILEData;
import atlantis.data.ATrackData;
import atlantis.graphics.ACursorFactory;
import atlantis.graphics.APickingGraphics2D;
import atlantis.utils.AMath;



public class APickInteraction extends AInteraction implements AMousePressListener, AEnterExitListener
{
    
    public APickInteraction()
    {
        super(0, NO_REPAINT, SCREEN_COORDINATES, false);
    }
    
    
    
    public int getPressButton()
    {
        return AInteractionsManager.ALL;
    }
    
    
    
    private void showCaloPulseShapePlot(AList picked, boolean withcurve)
    {
        Integer i = APickingGraphics2D.getPickedHitIndex();
        if(i != null)
        {
            ACalorimeterData c = (ACalorimeterData) picked.getSource();
            c.plotPulseShapes(i.intValue(), withcurve);
        }
    }
    
    
    
    private void showTriggerPulseShapePlot(AList picked)
    {
        Integer i = APickingGraphics2D.getPickedHitIndex();
        if(i != null)
        {
            ALVL1TriggerTowerData t = (ALVL1TriggerTowerData) picked.getSource();
            t.plotPulseShapes(i.intValue());
        }
    }
    
    
    
    private void ignorePickedItem(AList picked)
    {
        Integer i = APickingGraphics2D.getPickedHitIndex();
        if(i != null)
        {
            AData d = (AData) picked.getSource();
            String k = d.getStoreGateKey() != null ? ":" + d.getStoreGateKey() : "";
            String m = "\n\n" + d.getNameScreenName() + k + " ID " +
            d.getIdFromIndex(i.intValue()) + " (index " + i.intValue() +
            ")  will be invisible and ignored " +
            "(until removed from the Invisible list)";
            AOutput.alwaysAppend(m, ALogInterface.WARNING);
            AListManager.getInstance().addInvisible(picked);
        }
        
    }
    
    
    
    private void showMuonSector(AList picked)
    {
        if(picked.getSource() instanceof AMuonHitData)
        {
            int sector = ((AMuonHitData) picked.getSource()).getSector(picked.getItems()[0]);
            int signRZ = ((AMuonHitData) picked.getSource()).getRZSign(picked.getItems()[0]);
            if(signRZ == -1)
            {
                sector = (sector + 8) % 16;
            }
            parameterStore.get("YZ", "Phi").setD(sector * 22.5);
            window.repaintFromScratch();
        }
    }
    
    
    
    private void showTrackResidualPlot(AList picked)
    {
        AData pickedData = picked.getSource();
        if(pickedData instanceof ATrackData)
        {
            if(((ATrackData)pickedData).getResidualStatus())
            {
                APickingGraphics2D.showTrackResiduals(window,
                                                      (int) APickingGraphics2D.getPickedH(), (int) APickingGraphics2D.getPickedV());
            }
            else
            {
                AOutput.append("\nNo residual information available for this track\n",
                               ALogInterface.PICK);
            }
        }
    }
    
    private void showJetView(AList picked)
    {
        AData pickedData = picked.getSource();
        if(pickedData instanceof AJetData){
            
            // Use window "B" to show jets
            String jetWindowName = "B";
            if(!ACanvas.getCanvas().isValidWindowName(jetWindowName))
            {
                AOutput.append("\nWindow " + jetWindowName + " is not available in current layout"
                               + "\n", ALogInterface.WARNING);
            }
            else if(window.getName().equals(jetWindowName)){
                AOutput.append("\nPlease select jet view from a window other than window " + jetWindowName, ALogInterface.WARNING);
            }
            else{
                AWindow jetWindow = ACanvas.getCanvas().getWindow(jetWindowName);
                
                //set as NYX to get transition
                AProjectionNPhysics jetProj =
                (AProjectionNPhysics) AProjectionsManager.getProjection("NPhysics");
                
                //setup the projection
                ACanvas.getCanvas().moveToFrontWindow(jetWindowName);
                jetWindow.setProjection(jetProj);
                
                //configure the projection
                jetProj.configureFrame(ANManager.getManager().getFrameManager(jetWindow));
                ANProjectionPhysics finalProj = (ANProjectionPhysics) ANManager.getManager().getFrameManager(jetWindow).getTargetProjection();
                
                AJetData jd = ((AJetData) pickedData);
                int index=picked.getItems()[0];
                finalProj.lookAt(jd, index);
            }
        }
    }
    
    
    /**
     * @param picked AList
     * Check if picked data is SimVertex or RecVertex (RVx) and use the values
     * as Atlantis primary vertex: Projection->eta/phi menu
     */
    private void setAtlantisPrimaryVertex(AList picked)
    {
        Integer i = APickingGraphics2D.getPickedHitIndex();
        AData pickedData = picked.getSource();
        double[] vtx = null;
        
        if(i != null)
        {
            if(pickedData instanceof ARVxData)
            {
                vtx = ((ARVxData) pickedData).getVertex(i.intValue());
            }
            if(pickedData instanceof ASVxData)
            {
                vtx = ((ASVxData) pickedData).getVertex(i.intValue());
            }
            
            if(vtx != null)
            {
                parameterStore.get("Event", "XVtx").setD(vtx[0]);
                parameterStore.get("Event", "YVtx").setD(vtx[1]);
                parameterStore.get("Event", "ZVtx").setD(vtx[2]);
                AOutput.append("\n\nPrimary vertex set: Projection->" +
                               AMath.PHI + AMath.ETA + ":\n" +
                               "  XVtx = " + String.format("%.5f",vtx[0]) + " cm\n" +
                               "  YVtx = " + String.format("%.5f",vtx[1]) + " cm\n" +
                               "  ZVtx = " + String.format("%.5f",vtx[2]) + " cm\n",
                               ALogInterface.WARNING);
            }
        }
        
    } // setAtlantisPrimaryVertex() -----------------------------------------
    
    
    
    public void pressed(Point2D.Double p, int button, int key)
    {
        Point ps = null;
        Point pw = null;
        int pickingMode = parameterStore.get("Event", "PickingMode").getI();
        
        if(button == AInteractionsManager.LEFT)
        {
            if(key == KeyEvent.VK_C)
            {
                AListManager.getInstance().clearHighlightAndMassCalculation();
            }
            else
            {
                Rectangle bounds = window.getBounds();
                bounds.setLocation(0, 0);
                window.paintWindowFromScratch(new APickingGraphics2D(bounds,
                                                                     pickingMode, new Point((int) p.x, (int) p.y)));
                
                // this call prints pick information (calls datatype's getHitInfo())
                if(!(key == KeyEvent.VK_L))
                {
                	APickingGraphics2D.printPicked();
                }
                AList picked = APickingGraphics2D.getPickedHit();
                
                if(picked != null && pickingMode == APickingGraphics2D.PICK_HITS_AND_TRACKS)
                {
                    
                    // highlight by default (not only when key==KeyEvent.VK_H)
                    AListManager.getInstance().highlight(picked);
                    
                    // try to always show calo pulse shapes plot by default
                    // if no key modifier is pressed
                    if(picked.getSource() instanceof ACalorimeterData &&
                       key == KeyEvent.KEY_LOCATION_UNKNOWN)
                    {
                        showCaloPulseShapePlot(picked, true);
                    }
                    // 2009-01-19 for LAr only, see comment at
                    // the ALArData.plotADCCounts() method
                    if(picked.getSource() instanceof ACalorimeterData &&
                       key == KeyEvent.VK_D)
                    {
                        showCaloPulseShapePlot(picked, false);
                    }
                    if(picked.getSource() instanceof ATILEData &&
                       key == KeyEvent.VK_T) {
                        
                        Integer i = APickingGraphics2D.getPickedHitIndex();
                        if (i != null) {
                            ATILEData c = (ATILEData) picked.getSource();
                            c.plotPulseShapesWithTiming(i.intValue());
                        }
                    }
                    if(picked.getSource() instanceof ALVL1TriggerTowerData &&
                       key == KeyEvent.KEY_LOCATION_UNKNOWN)
                    {
                        showTriggerPulseShapePlot(picked);
                    }
                    
                    if(picked.getSource() instanceof AJetData)
                    {
                        double phi = ((AJetData) picked.getSource()).getPhi(picked.getItems()[0]);
                        parameterStore.get("XZ", "Phi").setD(Math.toDegrees(phi));
                    }
                    
                    switch(key)
                    {
                        case KeyEvent.VK_A:
                            AListManager.getInstance().add(picked);
                            break;
                        case KeyEvent.VK_S:
                            showMuonSector(picked);
                            break;
                        case KeyEvent.VK_N:
                            APickingGraphics2D.navigatePicked();
                            break;
                        case KeyEvent.VK_R:
                            showTrackResidualPlot(picked);
                            break;
                        case KeyEvent.VK_V:
                            setAtlantisPrimaryVertex(picked);
                            break;
                        case KeyEvent.VK_I:
                            ignorePickedItem(picked);
                            break;
                        case KeyEvent.VK_J:
                            if(parameterStore.get("Prefs", "OpenGL").getStatus()){
                                showJetView(picked);
                            }
                            break;
                        case KeyEvent.VK_M:
                            AListManager.getInstance().massCalc(picked);
                            break;
                        case KeyEvent.VK_L: // Mass calculation for Neutral Decay scenarios
                        	AListManager.getInstance().massCalcV0(picked);
                        	break;
                        case KeyEvent.VK_P: // Calculate delta Phi for picked objects
                        	AListManager.getInstance().deltaR(picked);
                        	break;
                        	
                            
                    } // switch
                    
                }
                else if(pickingMode == APickingGraphics2D.PICK_DETECTORS)
                {
                    AListManager.getInstance().clearHighlight();
                }
            }
            
        } // if(button == AInteractionsManager.LEFT)
        
        else if(button == AInteractionsManager.RIGHT)
        {
            Rectangle bounds = window.getBounds();
            bounds.setLocation(0, 0);
            window.paintWindowFromScratch(new APickingGraphics2D(bounds, APickingGraphics2D.MOVE,
                                                                 new Point((int) p.x, (int) p.y)));
        }
        
        if(pickingMode != APickingGraphics2D.PICK_DETECTORS)
        {
            if(APickingGraphics2D.getPickedData() != null
               || APickingGraphics2D.getPickedResidual() != null)
            {
                /* int numEvents = Atlantis.getEventManager().getNumberOfEvents();
                 if(numEvents > 0)
                 {
                 AOutput.append("\n(" + ++numEvents + " events currently displayed)\n", ALogPane.PICK);
                 }*/
                
                //TODO fix to avoid problems with nge
                if(window==null)
                    return;
                
                pw = new Point(APickingGraphics2D.getPickedH(), APickingGraphics2D.getPickedV());
                if(pw.x != 0 && pw.y != 0)
                    try
                {
                    ps = new Point(pw);
                    SwingUtilities.convertPointToScreen(ps, window);
                    new Robot().mouseMove(ps.x, ps.y);
                }
                catch(AWTException e)
                {}
                else
                {
                    AOutput.alwaysAppend("\n\nYou must pick before you can move", ALogInterface.BAD_COMMAND);
                }
            }
            else
            {
                if(!(window.getProjection() instanceof AProjectionTrackResidual))
                    AOutput.alwaysAppend("\n\nNothing to pick", ALogInterface.BAD_COMMAND);
            }
        }
        
    } // pressed() ------------------------------------------------------------
    
    
    public void paint(Graphics2D g)
    {}
    
    /**
     * Gets called each time we enter a window in which this interaction is active
     */
    public void entered()
    {
        //Set mouse cursor
        window.setCursor(ACursorFactory.getInstance().getPickCursor());
    }
    
    /**
     * Gets called each time we leave a window in which this interaction is active
     */
    public void exited()
    {
        //Set mouse cursor
        window.setCursor(ACursorFactory.getInstance().getDefaultCursor());
    }
    
    public AModifier[] getModifiers()
    {
        AModifier[] mods = new AModifier[] {
            new AModifier(KeyEvent.VK_UNDEFINED, false, "Pick"),
            new AModifier(KeyEvent.VK_UNDEFINED, true, "Move to last picked"),
            new AModifier(KeyEvent.VK_A, false, "Add to the current (active) list"),
            new AModifier(KeyEvent.VK_C, false, "Clear highlighted and calculation lists"),
            new AModifier(KeyEvent.VK_D, false, "Plot ADC counts (digits) only"),
            new AModifier(KeyEvent.VK_T, false, "Plot raw and cell time pulses (TILE only)"),
            new AModifier(KeyEvent.VK_I, false, "Ignore (make invisible) picked hits/tracks"),
            new AModifier(KeyEvent.VK_L, false, "Add to the V0 mass calculation list"),
            new AModifier(KeyEvent.VK_M, false, "Add to the mass calculation list"),    
            new AModifier(KeyEvent.VK_P, false, "Calculate Delta R for 2 tracks"),
            new AModifier(KeyEvent.VK_N, false, "Navigate (show data associations)"),
            new AModifier(KeyEvent.VK_R, false, "Plot track residuals if available"),
            new AModifier(KeyEvent.VK_S, false, "Select this muon Sector"),
            new AModifier(KeyEvent.VK_V, false, "Set prim.vertex from picked rec/sim vertex"),
            new AModifier(KeyEvent.VK_J, false, "Jet view (OpenGL only)")
        };
        
        return mods;
        
    }
}

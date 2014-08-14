package atlantis.interactions;

import java.awt.Rectangle;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.geom.Point2D;
import javax.swing.JMenuItem;
import java.util.List;
import java.util.ArrayList;

import atlantis.event.AData;
import atlantis.event.AEventManager;
import atlantis.canvas.ACanvas;
import atlantis.canvas.AWindow;
import atlantis.data.ACalorimeterData;
import atlantis.event.AEvent;
import atlantis.graphics.dnd.ADnDLabel;
import atlantis.graphics.dnd.ADragListener;
import atlantis.output.ALogInterface;
import atlantis.output.AOutput;
import atlantis.parameters.APar;
import atlantis.parameters.AParameter;
import atlantis.projection.AProjection;
import atlantis.projection.AProjectionFR;
import atlantis.projection.AProjectionFZ;
import atlantis.projection.AProjectionRZ;
import atlantis.projection.AProjectionVP;
import atlantis.projection.AProjectionXZ;
import atlantis.projection.AProjectionYX;
import atlantis.projection.AProjectionYZ;
import atlantis.utils.AMath;
import atlantis.utils.A3Vector;

public class ARectangleVPSelection extends ARectangleSelection
                                   implements ActionListener, ADragListener
{

    public final static String RUBBERBAND_WHOLE_WINDOW = "Rubberband Whole Window";
    public final static String ZOOM_LAR = "Zoom LAr";
    public final static String ZOOM_CALORIMETERS = "Zoom Calorimeters";
    public final static String CUT_ETAPHI = "Cut  ( " + AMath.ETA + AMath.PHI + " cut )";
    public final static String PRINT_CONTENTS = "Print Contents";
    public final static String SHOW_IN_3DBOX = "Show in 3DBox";
    public final static String TURN_OFF_ETAPHI_CUT = "Turn off " + CUT_ETAPHI;

    private static AEventManager eventManager = AEventManager.instance();

    public ARectangleVPSelection()
    {
        super();

        addActionButton(ZOOM_LAR, this, this);
        addActionButton(ZOOM_CALORIMETERS, this, this);
        addActionButton(CUT_ETAPHI, this, this);
        addActionButton(RUBBERBAND_WHOLE_WINDOW, this, this);
        addActionButton(PRINT_CONTENTS, this, this);
        addActionButton(SHOW_IN_3DBOX, this, this);

    }



  @Override
    public JMenuItem[] getPopupItems()
    {
        ArrayList al = new ArrayList();
        if (window.getUnzoomPossible())
        {
            JMenuItem item = new JMenuItem("Unzoom");
            al.add(item);
            item.addActionListener(new ActionListener()
            {
                public void actionPerformed(ActionEvent e)
                {
                    window.unzoom();
                }
            });
        }
        if(window.getUnzoomAllPossible(ZOOM_LAR))
        {
            JMenuItem item = new JMenuItem("Unzoom LAr");
            al.add(item);
            item.addActionListener(new ActionListener()
            {
                public void actionPerformed(ActionEvent e) {
                  window.unzoom(ARectangleVPSelection.ZOOM_LAR);
                }
             });
        }
        if(window.getUnzoomAllPossible(ZOOM_CALORIMETERS))
        {
            JMenuItem item = new JMenuItem("Unzoom Calorimeters");
            al.add(item);
            item.addActionListener(new ActionListener()
            {
                public void actionPerformed(ActionEvent e) {
                  window.unzoom(ZOOM_CALORIMETERS);
                }
             });
        }
        if (window.getUnzoomFullPossible())
        {
            JMenuItem item = new JMenuItem("Unzoom Full");
            al.add(item);
            item.addActionListener(new ActionListener()
                    {
                public void actionPerformed(ActionEvent e)
                {
                    window.unzoomFull();
                }
                    });
        }

        JMenuItem rww = new JMenuItem(RUBBERBAND_WHOLE_WINDOW);
        rww.addActionListener(this);

        JMenuItem toec = new JMenuItem(TURN_OFF_ETAPHI_CUT);
        toec.addActionListener(this);

        al.add(rww);
        al.add(toec);

        // Can't cast arrays very easily...
        JMenuItem[] ret = new JMenuItem[al.size()];
        for (int i = 0; i < al.size(); i++) {
            ret[i] = (JMenuItem)al.get(i);
        }

        return ret;
    }


    public void actionPerformed(ActionEvent e)
    {
        String action = e.getActionCommand();

        if(ZOOM_LAR.equals(action))
        {
            performZoom(ZOOM_LAR);
        }
        else if(ZOOM_CALORIMETERS.equals(action))
        {
            performZoom(ZOOM_CALORIMETERS);
        }
        else if(CUT_ETAPHI.equals(action))
        {
            applyCutIn(window.getName());
        }
        else if(RUBBERBAND_WHOLE_WINDOW.equals(action))
        {
            rubberbandWholeWindow();
        }
        else if(PRINT_CONTENTS.equals(action))
        {
            performOperationWithCuts(PRINT_CONTENTS);
        }
        else if(SHOW_IN_3DBOX.equals(action))
        {
            tracksTo3DBox();
        }
        else if(TURN_OFF_ETAPHI_CUT.equals(action))
        {
            turnOffEtaPhiCuts();
        }

    }


    private void turnOffEtaPhiCuts()
    {
        parameterStore.get("CutsATLAS", "CutPhi").setStatus(false);
        parameterStore.get("CutsATLAS", "CutEta").setStatus(false);
        String warning = "\n\nCuts -> ATLAS -> Cut " + AMath.ETA + " deactivated" +
                         "\nCuts -> ATLAS -> Cut " + AMath.PHI + " deactivated\n\n";
        AOutput.append(warning, ALogInterface.WARNING);
        ACanvas.getCanvas().repaintAllFromScratch();

    }



    private void tracksTo3DBox() {
        Point2D.Double[] corners = getCorners();
        double eta = (corners[2].x + corners[0].x) / 2.;
        double phi = Math.toRadians((corners[2].y + corners[0].y) / 2.);

        A3Vector ray = A3Vector.fromEtaPhiR(eta, phi, 1.0);

        // Need to make sure we change existing 3DBoxes
        parameterStore.get("3D", "xAxis").changeScope(AParameter.GLOBAL);
        parameterStore.get("3D", "yAxis").changeScope(AParameter.GLOBAL);
        parameterStore.get("3D", "zAxis").changeScope(AParameter.GLOBAL);
        parameterStore.get("3D", "xAxis").setD(ray.x);
        parameterStore.get("3D", "yAxis").setD(ray.y);
        parameterStore.get("3D", "zAxis").setD(ray.z);

        // 3DBoxes can be anywhere on the screen:
        ACanvas.getCanvas().repaintAllFromScratch();
    }


    public void dragPerformed(Object from, Object to, int action)
    {
        if (to instanceof ADnDLabel)
        {
            applyCutIn(((ADnDLabel) to).getText());
        }
        else if (to instanceof AWindow)
        {
            applyCutIn(((AWindow) to).getName());
        }
    }



    private boolean checkEtaPhiCutValues(double eta, double phi,
                                      double cutPhi, double cutEta)
    {
        if (!(parameterStore.get("CutsATLAS", "PhiMiddle").validateValue(phi) &&
              parameterStore.get("CutsATLAS", "EtaMiddle").validateValue(eta) &&
              parameterStore.get("CutsATLAS", "CutPhi").validateValue(cutPhi) &&
              parameterStore.get("CutsATLAS", "CutEta").validateValue(cutEta)))
          {
              String etaphi = AMath.ETA + AMath.PHI;
              AOutput.append("\nOperation cancelled: " + etaphi +
                             " cut values outside allowed range\n", ALogInterface.WARNING);
              return false;

          }
          else
          {
              return true;
          }

    }



    public void applyCutIn(String zoomWindowName)
    {
        Point2D.Double[] corners = getCorners();
        double eta = (corners[2].x + corners[0].x) / 2.;
        double cutEta = Math.abs((corners[2].x - corners[0].x)) / 2.;
        double phi = (corners[2].y + corners[0].y) / 2.;
        double cutPhi = Math.abs((corners[2].y - corners[0].y)) / 2.;
        double rhoVPlot = AProjectionVP.getRhoZoomVPlot();
        double zVPlot = AProjectionVP.getZZoomVPlot();
        double zVtx = parameterStore.get("Event", "ZVtx").getD();
        double zLower = Math.max(Math.min(rhoVPlot * AMath.tanLambda(eta - cutEta) + zVtx, 0.), -zVPlot);
        double zUpper = Math.min(Math.max(rhoVPlot * AMath.tanLambda(eta + cutEta) + zVtx, 0.), zVPlot);

        if(! checkEtaPhiCutValues(eta, phi, cutPhi, cutEta))
        {
            return;
        }

        String currentWindowName = window.getName();

        parameterStore.selectWindowParameters(zoomWindowName);
        parameterStore.get("CutsATLAS", "PhiMiddle").setD(phi);
        parameterStore.get("CutsATLAS", "EtaMiddle").setD(eta);
        parameterStore.get("CutsATLAS", "CutPhi").setD(cutPhi);
        parameterStore.get("CutsATLAS", "CutPhi").setStatus(true);
        parameterStore.get("CutsATLAS", "CutEta").setD(cutEta);
        parameterStore.get("CutsATLAS", "CutEta").setStatus(true);

        String warning = "\n\nCuts -> ATLAS -> Cut " + AMath.ETA + " now active" +
                         "\nCuts -> ATLAS -> Cut " + AMath.PHI + " now active\n\n";
        AOutput.append(warning, ALogInterface.WARNING);

        AWindow zoomWindow = ACanvas.getCanvas().getWindow(zoomWindowName);
        AProjection zoomProjection = zoomWindow.getProjection();

        if (zoomProjection instanceof AProjectionFR)
        {
            zoomWindow.setUserCorners(0., rhoVPlot, phi - cutPhi, phi + cutPhi);
        }
        else if (zoomProjection instanceof AProjectionFZ)
        {
            zoomWindow.setUserCorners(zLower, zUpper, phi - cutPhi, phi + cutPhi);
        }
        else if (zoomProjection instanceof AProjectionVP)
        {
            if (!zoomWindowName.equals(currentWindowName))
                zoomWindow.setUserCorners(eta - cutEta, eta + cutEta, phi - cutPhi, phi + cutPhi);
        }
        else if (zoomProjection instanceof AProjectionYX)
        {
            zoomWindow.setUserCorners(-rhoVPlot, rhoVPlot, -rhoVPlot, rhoVPlot);
        }
        else if (zoomProjection instanceof AProjectionXZ)
        {
            double rhoZoom = Math.min(rhoVPlot, 110.);

            zoomWindow.setUserCorners(zLower, zUpper, 0., rhoZoom);
            parameterStore.get("XZ", "Phi").setD(AMath.nearestPhiDegrees(phi));
        }
        else if (zoomProjection instanceof AProjectionYZ)
        {
            double rhoZoom = Math.min(rhoVPlot, 110.);

            zoomWindow.setUserCorners(zLower, zUpper, -rhoZoom, rhoZoom);
            parameterStore.get("YZ", "Phi").setD(AMath.nearestPhiDegrees(phi));
        }
        else if (zoomProjection instanceof AProjectionRZ)
        {
            zoomWindow.setUserCorners(zLower, zUpper, 0., rhoVPlot);
            parameterStore.get("RZ", "Phi").setD(AMath.nearestPhiDegrees(phi));
        }
        parameterStore.restoreWindowParameters();

        // don't repaint just the current window after cut(s) are applied,
        // but repaint everything (all windows)
        // zoomWindow.repaintFromScratch();
        ACanvas.getCanvas().repaintAllFromScratch();

    }

    public void performZoom(String typeOfZoom)
    {
        int numberOfWindows=0;

        if(ZOOM_LAR.equals(typeOfZoom))
        {
            numberOfWindows = 4;
        }
        else if(ZOOM_CALORIMETERS.equals(typeOfZoom))
        {
            numberOfWindows = 8;
        }

        AWindow currentWindow = window;
        String name = currentWindow.getName();
        currentWindow.saveLayout();
        boolean scaleStatus = currentWindow.getScaleStatus();
        Point2D.Double[] zoomedCorners = getCorners();
        layoutChange(typeOfZoom);
        int nameIndex=-1;
        for (int i = 0; i < numberOfWindows; i++)
        {
            String wName = Integer.toString(i + 1);
            AWindow w = ACanvas.getCanvas().getWindow(wName);
            //dont zoom current window as will not save corners correctly
            if (w != null && !name.equals(wName))
            {
                w.saveParameters(typeOfZoom);
                ACanvas.getCanvas().copyWindowSettings(currentWindow.getName(), wName);
                w.saveCorners(typeOfZoom);
                w.setUserCorners(zoomedCorners);
                w.setScaleStatus(scaleStatus);
                parameterStore.selectWindowParameters(wName);
                parameterStore.get("VP", "Mode").setI(AProjectionVP.MODE_ECAL_LAYER_0 + i);
                parameterStore.restoreWindowParameters();
                ACanvas.getCanvas().moveToFrontWindow(w.getName());
                w.repaintFromScratch();
            }
            else if(name.equals(wName))
            {
                nameIndex=i;
            }
        }
        //now do zoom of current window if needed
        if(nameIndex>0)
        {
            currentWindow.saveParameters(typeOfZoom);
            currentWindow.saveCorners(typeOfZoom);
            currentWindow.setUserCorners(zoomedCorners);
            currentWindow.setScaleStatus(scaleStatus);
            parameterStore.selectWindowParameters(name);
            parameterStore.get("VP", "Mode").setI(AProjectionVP.MODE_ECAL_LAYER_0 + nameIndex);
            parameterStore.restoreWindowParameters();
            ACanvas.getCanvas().moveToFrontWindow(name);
            currentWindow.repaintFromScratch();
        }
        for (int i = 0; i < numberOfWindows; i++)
            if (name.equals(Integer.toString(i + 1)))
                invalidate();
    }

    public void layoutChange(String typeOfZoom)
    {
        // change layout
        if(!ACanvas.getCanvas().getCurrentLayout().getName().equals("SQUARE"))
        {
            ACanvas.getCanvas().setCurrentLayout("SQUARE");
        }

        // print info message
        AOutput.append("\nWindows changed to: \n",ALogInterface.TITLE);
        AOutput.append("Window 1: ECAL sampling 0\n",ALogInterface.NORMAL);
        AOutput.append("Window 2: ECAL sampling 1\n",ALogInterface.NORMAL);
        AOutput.append("Window 3: ECAL sampling 2\n",ALogInterface.NORMAL);
        AOutput.append("Window 4: ECAL sampling 3\n",ALogInterface.NORMAL);

        if(ZOOM_CALORIMETERS.equals(typeOfZoom))
        {
            AOutput.append("Window 5: HCAL sampling 0\n",ALogInterface.NORMAL);
            AOutput.append("Window 6: HCAL sampling 1\n",ALogInterface.NORMAL);
            AOutput.append("Window 7: HCAL sampling 2\n",ALogInterface.NORMAL);
            AOutput.append("Window 8: HCAL sampling 3\n",ALogInterface.NORMAL);
        }
    }


    private void printContents()
    {
        // get all data within selection (eta/phi cut already applied)
        AEvent event = eventManager.getCurrentEvent();
        StringBuffer output = new StringBuffer("Data inside Rubberband selection:");
        // hits and tracks first
        List hitsAndTracks = event.getHitsAndTracks(window.getProjection());
        for(int i = 0; i < hitsAndTracks.size(); i++)
        {
            String info = ((AData) hitsAndTracks.get(i)).getVPHitInfo();
            if(! "".equals(info))
            {
                output.append("\n" + info);
            }
        }

        // calorimeters (as they are handled differently)
        List cal = event.getCalorimeters();
        for (int i = 0; i < cal.size(); i++)
        {
            ACalorimeterData calorimeter = (ACalorimeterData) cal.get(i);
            calorimeter.makeDrawList();
            String info = calorimeter.getVPHitInfo();
            if(! "".equals(info))
            {
                output.append("\n" + info);
            }
        }

        AOutput.alwaysAppend("\n" + output + "\n", ALogInterface.PICK);

    }



    /**
     * Apply eta phi cuts based on the selected (rubberband region), then
     * perform the desired operation with data in the selection and
     * restore previous cuts settings.
     */
    private void performOperationWithCuts(String operation)
    {
        // which corners fall into eta/phi rubberband selection
        Point2D.Double[] corners = getCorners();
        double eta = (corners[2].x + corners[0].x) / 2.;
        double cutEta = Math.abs((corners[2].x - corners[0].x)) / 2.;
        double phi = (corners[2].y + corners[0].y) / 2.;
        double cutPhi = Math.abs((corners[2].y - corners[0].y)) / 2.;

        if(! checkEtaPhiCutValues(eta, phi, cutPhi, cutEta))
        {
            return;
        }

        // need to temporarily apply eta/phi cut, but get the current
        // eta/phi cut values first and save them
        AParameter phiPar = parameterStore.get("CutsATLAS", "PhiMiddle");
        AParameter cutPhiPar = parameterStore.get("CutsATLAS", "CutPhi");
        AParameter etaPar = parameterStore.get("CutsATLAS", "EtaMiddle");
        AParameter cutEtaPar = parameterStore.get("CutsATLAS", "CutEta");
        double oldEta = etaPar.getD();
        double oldCutEta = cutEtaPar.getD();
        boolean oldCutEtaStatus = cutEtaPar.getStatus();
        double oldPhi = phiPar.getD();
        double oldCutPhi = cutPhiPar.getD();
        boolean oldCutPhiStatus = cutPhiPar.getStatus();

        // set new (temporary) values of eta/phi cuts according to selection
        phiPar.setD(phi);
        etaPar.setD(eta);
        cutPhiPar.setD(cutPhi);
        cutPhiPar.setStatus(true);
        cutEtaPar.setD(cutEta);
        cutEtaPar.setStatus(true);

        if(PRINT_CONTENTS.equals(operation))
        {
            printContents();
        }

        // set back the previous eta/phi cut values
        phiPar.setD(oldPhi);
        etaPar.setD(oldEta);
        cutPhiPar.setD(oldCutPhi);
        cutPhiPar.setStatus(oldCutPhiStatus);
        cutEtaPar.setD(oldCutEta);
        cutEtaPar.setStatus(oldCutEtaStatus);

    } // printContents() ----------------------------------------------------



    public void rubberbandWholeWindow()
    {
        Rectangle curr = window.getCurrDisp();

        double x = curr.getX();
        double y = curr.getY();
        double w = curr.getWidth();
        double h = curr.getHeight();

        setCenter(hr[0], x, y);
        setCenter(hr[1], x + w, y);
        setCenter(hr[2], x + w, y + h);
        setCenter(hr[3], x, y + h);
        isValid = true;
        region = 1;
        stop();
        window.repaint();
        oldAffectedRegion = curr;
        int safetySize = 5;

        oldAffectedRegion.x -= safetySize;
        oldAffectedRegion.y -= safetySize;
        oldAffectedRegion.width += 2 * safetySize;
        oldAffectedRegion.height += 2 * safetySize;

    }

}

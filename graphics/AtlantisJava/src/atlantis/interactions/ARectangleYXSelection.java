package atlantis.interactions;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.geom.Point2D;
import java.util.ArrayList;

import javax.swing.JMenuItem;

import atlantis.canvas.ACanvas;
import atlantis.canvas.AWindow;
import atlantis.graphics.dnd.ADragListener;
import atlantis.output.ALogInterface;
import atlantis.output.AOutput;
import atlantis.parameters.APar;
import atlantis.parameters.AParameter;
import atlantis.projection.AProjection;
import atlantis.projection.AProjectionYX;
import atlantis.projection.AProjectionsManager;

/**
 * Contains the function for selecting zoom endcap layers and summed endcaps
 * 
 * @author Mark Stockton
 */
public class ARectangleYXSelection extends ARectangleSelection implements ActionListener, ADragListener
{
    public final static String ZOOM_SUMMED = "summed";
    public final static String ZOOM_LAYERS = "layers";
    
    public ARectangleYXSelection()
    {
        super();
        addActionButton("Zoom Summed Endcaps", new ActionListener()                
        {
            public void actionPerformed(ActionEvent e)
            {
                if (e.getActionCommand().equals("Zoom Summed Endcaps"))
                    performZoom(ZOOM_SUMMED);
            }
        });
        addActionButton("Zoom Endcap Layers -", new ActionListener()                
        {
            public void actionPerformed(ActionEvent e)
            {
                if (e.getActionCommand().equals("Zoom Endcap Layers -"))
                    zoomLayersMinus();
            }
        });
        addActionButton("Zoom Endcap Layers +", new ActionListener()                
        {
            public void actionPerformed(ActionEvent e)
            {
                if (e.getActionCommand().equals("Zoom Endcap Layers +"))
                    zoomLayersPlus();
            }
        });
        addActionButton("Zoom Both Endcap Layers", new ActionListener()                
        {
            public void actionPerformed(ActionEvent e)
            {
                if (e.getActionCommand().equals("Zoom Both Endcap Layers"))
                    zoomLayersCombined();
            }
        });
    }

    public void zoomLayersMinus()
    {
        parameterStore.get("CutsCalo", "FCAL").setI(0);
        parameterStore.get("CutsCalo", "LAr").setI(0);
        parameterStore.get("CutsCalo", "HEC").setI(-2);
        performZoom(ZOOM_LAYERS);
    }
    
    public void zoomLayersPlus()
    {
        parameterStore.get("CutsCalo", "FCAL").setI(1);
        parameterStore.get("CutsCalo", "LAr").setI(1);
        parameterStore.get("CutsCalo", "HEC").setI(2);
        performZoom(ZOOM_LAYERS);
    }
    
    public void zoomLayersCombined()
    {
        parameterStore.get("CutsCalo", "FCAL").setI(-1);
        parameterStore.get("CutsCalo", "LAr").setI(-1);
        parameterStore.get("CutsCalo", "HEC").setI(-1);
        performZoom(ZOOM_LAYERS);
    }
    
    public void performZoom(String typeOfZoom)
    {
        int numberOfWindows=0;
        if(typeOfZoom.equals(ZOOM_SUMMED))
            numberOfWindows=4;
        else if(typeOfZoom.equals(ZOOM_LAYERS))
            numberOfWindows=8;
        
        AWindow currentWindow = window;
        String name = currentWindow.getName();
        currentWindow.saveLayout();
        boolean scaleStatus = currentWindow.getScaleStatus();
        Point2D.Double[] zoomedCorners = getCorners();
        //change layout and print info message
        layoutChange(typeOfZoom);
        int nameIndex=-1;
        for (int i = numberOfWindows; i >=1; i--)
        {
            String wName = Integer.toString(i);
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

                if(typeOfZoom.equals(ZOOM_SUMMED))
                    zoomSummedOptions(i-1);
                else if(typeOfZoom.equals(ZOOM_LAYERS))
                    parameterStore.get("YX", "Mode").setI(AProjectionYX.MODE_LAR_ENDCAP_PRESAMPLER + i -1);
               
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
            if(typeOfZoom.equals(ZOOM_SUMMED))
                zoomSummedOptions(nameIndex-1);
            else if(typeOfZoom.equals(ZOOM_LAYERS))
                parameterStore.get("YX", "Mode").setI(AProjectionYX.MODE_LAR_ENDCAP_PRESAMPLER + nameIndex -1);
            parameterStore.restoreWindowParameters();
            ACanvas.getCanvas().moveToFrontWindow(name);
            currentWindow.repaintFromScratch();
        }
        for (int i = 1; i <= numberOfWindows; i++)
            if (name.equals(Integer.toString(i)))
                invalidate();
        //if doing layers zoom last window shows rhoZ view
        if(typeOfZoom.equals(ZOOM_LAYERS))
            zoomLastWindow(scaleStatus, name);
    }
    
    public void layoutChange(String typeOfZoom)
    {
        if(typeOfZoom.equals(ZOOM_SUMMED))
        {
            //change layout
            if(!ACanvas.getCanvas().getCurrentLayout().getName().equals("FOUR SQUARES"))
                ACanvas.getCanvas().setCurrentLayout("FOUR SQUARES");
            //print info message
            AOutput.append("\nWindows changed to:\n",ALogInterface.TITLE);
            AOutput.append("Window 1: LAr Endcap -\n",ALogInterface.NORMAL);
            AOutput.append("Window 2: LAr Endcap +\n",ALogInterface.NORMAL);
            AOutput.append("Window 3: HEC -\n",ALogInterface.NORMAL);
            AOutput.append("Window 4: HEC +\n",ALogInterface.NORMAL);
        }
        else if(typeOfZoom.equals(ZOOM_LAYERS))
        {
            //change layout
            if(!ACanvas.getCanvas().getCurrentLayout().getName().equals("SQUARE"))
                ACanvas.getCanvas().setCurrentLayout("SQUARE");
            //print info message
            AOutput.append("\nWindows changed to:\n",ALogInterface.TITLE);
            AOutput.append("Window 1: LAr Endcap Presampler\n",ALogInterface.NORMAL);
            AOutput.append("Window 2: LAr Endcap 1\n",ALogInterface.NORMAL);
            AOutput.append("Window 3: LAr Endcap 2\n",ALogInterface.NORMAL);
            AOutput.append("Window 4: LAr Endcap 3\n",ALogInterface.NORMAL);
            AOutput.append("Window 5: HEC 1\n",ALogInterface.NORMAL);
            AOutput.append("Window 6: HEC 2\n",ALogInterface.NORMAL);
            AOutput.append("Window 7: HEC 3\n",ALogInterface.NORMAL);
            AOutput.append("Window 8: HEC 4\n",ALogInterface.NORMAL);
            AOutput.append("Window 9: rhoZ view showing both endcaps\n",ALogInterface.NORMAL);
        }
    }
    
    public void zoomSummedOptions(int i)
    {
        parameterStore.get("CutsCalo", "LAr").changeScope(AParameter.LOCAL);
        parameterStore.get("CutsCalo", "HEC").changeScope(AParameter.LOCAL);
        switch (i)
        {
            case 0:
                parameterStore.get("CutsCalo", "LAr").setI(0);
                parameterStore.get("CutsCalo", "HEC").setI(-1);
                break;
            case 1:
                parameterStore.get("CutsCalo", "LAr").setI(1);
                parameterStore.get("CutsCalo", "HEC").setI(-1);
                break;
            case 2:
                parameterStore.get("CutsCalo", "LAr").setI(-1);
                parameterStore.get("CutsCalo", "HEC").setI(-2);
                break;
            case 3:
                parameterStore.get("CutsCalo", "LAr").setI(-1);
                parameterStore.get("CutsCalo", "HEC").setI(2);
                break;
        }
        parameterStore.get("YX", "Mode").setI(AProjectionYX.MODE_LAR_ENDCAP_SUMMED + (int) Math.floor(i/2));
    }
    
    public void zoomLastWindow(boolean scaleStatus, String name)
    {
        String wName = Integer.toString(9);
        AWindow w = ACanvas.getCanvas().getWindow(wName);
        //want last window to be RZ view but won't change its zoom                    
        AProjection p = AProjectionsManager.getProjection("RZ");
        w.setProjection(p);
        Point2D.Double[] newCorners = new Point2D.Double[3];
        //FIXME hardcoded geometry of endcaps
        //set region to be round the endcaps
        newCorners[0]=new Point2D.Double(-650.0, 225);
        newCorners[1]=new Point2D.Double(650.0, 225);
        newCorners[2]=new Point2D.Double(650.0, -225);
        
        w.setUserCorners(newCorners);
        w.setScaleStatus(scaleStatus);
        parameterStore.selectWindowParameters(wName);
        parameterStore.restoreWindowParameters();
        ACanvas.getCanvas().moveToFrontWindow(w.getName());
        w.repaintFromScratch();
        if (name.equals(wName))
                invalidate();
    }
    
    public void dragPerformed(Object from, Object to, int action)
    {}
    
    public JMenuItem[] getPopupItems() {
        ArrayList al = new ArrayList();
        if(window.getUnzoomPossible())
        {
            JMenuItem it = new JMenuItem("Unzoom");
            it.addActionListener(new ActionListener()
            {
                public void actionPerformed(ActionEvent e) 
                {
                    window.unzoom();
                }
            });
            al.add(it);
        }
        if(window.getUnzoomAllPossible(ZOOM_LAYERS))
        {
            JMenuItem it = new JMenuItem("Unzoom layers");
            it.addActionListener(new ActionListener()
            {
                public void actionPerformed(ActionEvent e) 
                {
                    window.unzoom(ZOOM_LAYERS);
                }
            });
            al.add(it);
        }
        if(window.getUnzoomAllPossible(ZOOM_SUMMED))
        {
            JMenuItem it = new JMenuItem("Unzoom summed");
            it.addActionListener(new ActionListener()
            {       
                public void actionPerformed(ActionEvent e) 
                {
                    window.unzoom(ZOOM_SUMMED);
                }
            });
            al.add(it);
        }
        if (window.getUnzoomFullPossible())
        {
            JMenuItem it = new JMenuItem("Unzoom Full");
            it.addActionListener(new ActionListener()
            {
                public void actionPerformed(ActionEvent e)
                {
                    window.unzoomFull();
                }
            });
            al.add(it);
        }
        // Can't cast arrays very easily...
        JMenuItem[] ret = new JMenuItem[al.size()];
        for(int i = 0; i < al.size(); i++)
        {
            ret[i] = (JMenuItem) (al.get(i));
        }
        return ret;
    }
    
    public void actionPerformed(ActionEvent e) {
        String action=e.getActionCommand();
        if(action.equals("Unzoom")) {
            window.unzoom();
        }
        if(action.equals("Unzoom layers")) {
            window.unzoom(ZOOM_LAYERS);
        }
        if(action.equals("Unzoom summed")) {
            window.unzoom(ZOOM_SUMMED);
        }
        if(action.equals("Unzoom Full")) {
            window.unzoomFull();
        }
    }
}

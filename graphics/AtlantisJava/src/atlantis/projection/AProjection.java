package atlantis.projection;


import java.awt.event.ActionListener;
import atlantis.canvas.AWindow;
import atlantis.event.AEventManager;
import atlantis.parameters.APar;

import javax.swing.*;
import java.awt.*;


public abstract class AProjection implements ActionListener {
	
  protected static APar parameterStore = APar.instance();
  protected static AEventManager eventManager = AEventManager.instance();
    
  public abstract String getName();
  public abstract String getScreenName();
  public abstract void paint(AWindow window, Graphics g);
  
  public abstract JMenuItem[] getPopupItems();
  
  // next 4/5 needed for normal 2d projections, not for others e.g. Braintests
  // should be changed at some point.....
  public abstract String getXLabel();
  public abstract String getYLabel();
  public abstract String getXUnits();
  public abstract String getYUnits();
  public abstract void setScales();
  public abstract boolean processLocalCommand(String name);
}

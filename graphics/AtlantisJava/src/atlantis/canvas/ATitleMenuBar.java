package atlantis.canvas;

import javax.swing.JLabel;
import javax.swing.JMenuBar;
import java.awt.Font;
import java.awt.FontMetrics;
import java.awt.Graphics;

import atlantis.event.AEvent;
import atlantis.event.AEventManager;
import atlantis.graphics.AGraphics;
import atlantis.parameters.APar;


public class ATitleMenuBar extends JMenuBar 
{

  public ATitleMenuBar() 
  {
    // just to set the height of the title bar

    JLabel atlas = new JLabel(" Atlantis ");
    atlas.setFont(new Font("Courier", Font.PLAIN, 25));
    add(atlas);
  }

  @Override
  public void paint(Graphics g) 
  {
	AEventManager eventManager = AEventManager.instance();
    AEvent event = eventManager.getCurrentEvent();
    Font titleFont=new Font("Dialog", Font.BOLD, 15);
    int availableSpace = (int) Math.floor(getWidth()-10);
    FontMetrics fm = ACanvas.getCanvas().getFontMetrics(titleFont);
    
    AGraphics ag=AGraphics.makeAGraphics(g);
    ag.setColor(AWindow.BORDER_BACKGROUND_COLOR);
    ag.fillRect(0, 0, getWidth(), getHeight());
    ag.setColor(AWindow.BORDER_FOREGROUND_COLOR);
    ag.updateColor();
    ag.setFont(titleFont);
    availableSpace -= fm.stringWidth("ATLAS  Atlantis");
    ag.drawString("ATLAS ", 5, getHeight()-10);
    ag.drawString(" Atlantis", getWidth()-fm.stringWidth(" Atlantis")-5, getHeight()-10);
    
    String[] titleSections = new String[6];
    if(event != null)
    {
      titleSections[0] = !event.getDateTime().equals("n/a") ? event.getDateTime() : "";
      titleSections[1] = " source:" + event.getSourceName();
      int extension_dot = titleSections[1].toLowerCase().lastIndexOf(".");
      if(extension_dot > 0)
        titleSections[1] = titleSections[1].substring(0,extension_dot);
      titleSections[2] = " run:" + event.getRunNumber();
      titleSections[3] = " ev:" + event.getEventNumber();
      titleSections[4] = " lumiBlock:" + event.getLumiBlock();
      titleSections[5] = " eventProperties: " + event.getEventProperties();
    }
    else
    {
      titleSections[0]="";
      titleSections[1]=" event:n/a";
      titleSections[2]=" run:n/a";
      titleSections[3]=" ev:n/a";
      titleSections[4]="";
      titleSections[5]="";
    }
    

    //for masterclass turn off some of the canvas title parts
    APar parameterStore = APar.instance();
    if(parameterStore.get("Minerva","hiderun").getStatus())
      titleSections[2]="";
    if(parameterStore.get("Minerva","hideev").getStatus())
      titleSections[3]="";

 
    StringBuilder outputString = new StringBuilder();
    int titleSectionSize=0;
    for(int i=0; i<6; i++)
    {
      if(titleSectionSize+fm.stringWidth(titleSections[i])<availableSpace)
      {
	  if(!titleSections[i].endsWith("default")){
	      titleSectionSize+=fm.stringWidth(titleSections[i]);
	      outputString.append(titleSections[i]);
	  }
      }
    }
    ag.drawString(outputString.toString(), (getWidth()-titleSectionSize)/2, getHeight()-10);
  }
}

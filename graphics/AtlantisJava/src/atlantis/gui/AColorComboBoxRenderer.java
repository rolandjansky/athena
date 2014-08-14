package atlantis.gui;

import java.awt.Color;
import java.awt.Component;
import java.awt.Dimension;
import java.awt.Graphics;
import javax.swing.Icon;
import javax.swing.JLabel;
import javax.swing.ToolTipManager;
import javax.swing.BorderFactory;
import javax.swing.JList;
import javax.swing.ListCellRenderer;
import javax.swing.border.Border;

import atlantis.graphics.colormap.AColorMap;

/**
 * The class used as a renderer when displaying the colors in the UI.
 */
public class AColorComboBoxRenderer extends JLabel
  implements ListCellRenderer {

  private ColorIcon icon=new ColorIcon();
  private Border selectedBorder;
  private Border deselectedBorder=BorderFactory.createEmptyBorder(2, 2, 2, 2);

  public Component getListCellRendererComponent(
      JList list, Object value, int index,
      boolean isSelected, boolean cellHasFocus) {

    int colorIndex=((Integer)value).intValue();
    Color[] colorMap=AColorMap.getColors();
    Color color=colorMap[colorIndex];

    icon.setColor(color);
    setIcon(icon);
    setText(""+((Integer)value).intValue());

    if(isSelected) {
      selectedBorder=BorderFactory.createLineBorder(list.getSelectionBackground(), 2);

      setBorder(selectedBorder);

      ToolTipManager.sharedInstance().setInitialDelay(1);
      ToolTipManager.sharedInstance().setReshowDelay(1);

      String message;

      if(color.equals(Color.black))
        message="Black: ";
      else if(color.equals(Color.blue))
        message="Blue: ";
      else if(color.equals(Color.cyan))
        message="Cyan: ";
      else if(color.equals(Color.darkGray))
        message="DarkGray: ";
      else if(color.equals(Color.gray))
        message="Gray: ";
      else if(color.equals(Color.green))
        message="Green: ";
      else if(color.equals(Color.lightGray))
        message="LightGray: ";
      else if(color.equals(Color.magenta))
        message="Magenta: ";
      else if(color.equals(Color.orange))
        message="Orange: ";
      else if(color.equals(Color.pink))
        message="Pink: ";
      else if(color.equals(Color.red))
        message="Red: ";
      else if(color.equals(Color.white))
        message="White: ";
      else if(color.equals(Color.yellow))
        message="Yellow: ";
      else
        message="";

      list.setToolTipText(message+color.getRed()+", "+color.getGreen()+", "+color.getBlue());
    } else setBorder(deselectedBorder);

    return this;
  }

  private static class ColorIcon implements Icon {
	  private int w, h;
	  private Color color;

	  public ColorIcon() {
		  this(Color.gray, 50, 15);
	  }

	  public ColorIcon(Dimension d) {
		  this.w=d.width;
		  this.h=d.height;
	  }

	  public ColorIcon(Color color, int w, int h) {
		  this.color=color;
		  this.w=w;
		  this.h=h;
	  }

	  public void paintIcon(Component c, Graphics g, int x, int y) {
		  g.setColor(Color.black);
		  g.drawRect(x, y, w-1, h-1);
		  g.setColor(color);
		  g.fillRect(x+1, y+1, w-2, h-2);
	  }

	  public Color getColor() {
		  return color;
	  }

	  public void setColor(Color color) {
		  this.color=color;
	  }

	  public int getIconWidth() {
		  return w;
	  }

	  public int getIconHeight() {
		  return h;
	  }
  }
}

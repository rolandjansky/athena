package atlantis.list;


import javax.swing.Icon;
import java.awt.*;

import atlantis.graphics.colormap.AColorMap;


// icon with size and color

class AColorIcon implements Icon {
  private int w, h;
  private int colorIndex;

  public AColorIcon() {
    this(AColorMap.WH, 50, 15);
  }

  public AColorIcon(Dimension d) {
    this.w=d.width;
    this.h=d.height;
  }

  public AColorIcon(int colorIndex, int w, int h) {
    this.colorIndex=colorIndex;
    this.w=w;
    this.h=h;
  }

  public void paintIcon(Component c, Graphics g, int x, int y) {
    g.setColor(Color.black);
    g.drawRect(x, y, w-1, h-1);
    if(colorIndex==AColorMap.INVISIBLE) {
      g.setColor(AColorMap.getColors()[AColorMap.WH]);
      g.fillRect(x+1, y+1, w-2, h-2);
      g.setColor(Color.black);
      g.drawLine(x, y, x+w-1, y+h-1);
      g.drawLine(x, y+h-1, x+w-1, y);
    } else if(colorIndex==AColorMap.NO_COLOR) {
      g.setColor(AColorMap.getColors()[AColorMap.WH]);
      g.fillRect(x+1, y+1, w-2, h-2);
      g.setColor(Color.black);
      int xc=x+w/2;
      int yc=y+h/2;
      g.drawLine(xc, yc-2, xc, yc+2);
      g.drawLine(xc-2, yc, xc+2, yc);
    } else {
      g.setColor(AColorMap.getColors()[colorIndex]);
      g.fillRect(x+1, y+1, w-2, h-2);
    }
  }

  public int getColorIndex() {
    return colorIndex;
  }

  public void setColorIndex(int colorIndex) {
    this.colorIndex=colorIndex;
  }

  public int getIconWidth() {
    return w;
  }

  public int getIconHeight() {
    return h;
  }
}

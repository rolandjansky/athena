package atlantis.graphics.layout;

import java.awt.Container;
import java.awt.Insets;
import java.awt.LayoutManager;
import java.awt.Component;
import java.awt.Dimension;

/**
 * This Layout manager aranges the components in a flow like awt.FlowLayout.
 * The difference is that this layout resizes the container to the minimum
 * size after every layout operation, and therefore ensures that all components
 * it laysout are visible. Nothing to do with the layout of the Atlantis canvas.
 */

public class AFlowLayout implements LayoutManager {
  private int hgap;
  private int vgap;

  public AFlowLayout(int hgap, int vgap) {
    this.hgap=hgap;
    this.vgap=vgap;
  }

  public void addLayoutComponent(String name, Component comp) {}

  public void removeLayoutComponent(Component comp) {}

  public Dimension minimumLayoutSize(Container target) {
    return preferredLayoutSize(target);
  }

  public Dimension preferredLayoutSize(Container target) {
    Insets insets=target.getInsets();
    Dimension D=target.getSize();
    int W=D.width-insets.left-insets.right;
    int L=hgap;
    int y=vgap;
    int w, h, hmax=0;

    for(int i=0; i<target.getComponentCount(); i++) {
      Dimension d=target.getComponent(i).getPreferredSize();

      w=d.width;
      h=d.height;

      if(L+w+hgap<W) {
        L+=w+hgap;
        if(h>hmax) hmax=h;
      } else {
        L=hgap+w+hgap;
        y+=hmax+vgap;
        hmax=0;
        if(h>hmax) hmax=h;
      }
    }
    if(target.getComponentCount()>0)
      y+=hmax+vgap;

    return new Dimension(W, y);
  }

  public void layoutContainer(Container target) {
    Insets insets=target.getInsets();
    Dimension D=target.getSize();
    int W=D.width-insets.left-insets.right;
    int L=hgap;
    int y=vgap;
    int from=0;
    int to=0;
    int w, h, hmax=0;

    for(int i=0; i<target.getComponentCount(); i++) {
      Dimension d=target.getComponent(i).getPreferredSize();

      w=d.width;
      h=d.height;

      if(L+w+hgap<W) {
        L+=w+hgap;
        to=i;
        if(h>hmax) hmax=h;
      } else {
        layout(target, hgap+insets.left+(W/2-L/2), y, from, to);
        L=hgap+w+hgap;
        from=i;
        to=i;
        y+=hmax+vgap;
        hmax=0;
        if(h>hmax) hmax=h;
      }
    }

    if(target.getComponentCount()>0)
      layout(target, hgap+insets.left+(W/2-L/2), y, from, to);

    /*
     if (y + hmax + vgap != D.height)
     SwingUtilities.invokeLater(new ARunAnotherLayout(target));
     */
  }

  public void layout(Container target, int x, int y, int from, int to) {
    int xi=x;

    for(int i=from; i<=to; i++) {
      Component comp=target.getComponent(i);
      Dimension d=comp.getPreferredSize();

      comp.setBounds(xi, y, d.width, d.height);
      xi+=d.width+hgap;
    }
  }

}

package atlantis.gui;

import java.awt.Graphics;
import javax.swing.JPanel;

/**
 * LazyPanel is an abstract base class that provides functionality
 * to defer populating a Panel object until it is actually viewed.
 * This is extremely useful when using CardLayout and tab panel
 * views because it allows the construction of the subviews to
 * be done on a pay-as-you-go basis instead of absorbing all the cost
 * of construction up front.
 *
 * If subclasses choose to override any of the following methods,
 * it is their responsibility to ensure their overridden methods
 * call the parent's method first. The methods are:
 *
 * public void paint (Graphics)
 * public void paintComponents(Graphics)
 * public void paintAll (Graphics)
 * public void repaint ()
 * public void repaint (long)
 * public void repaint (int, int, int, int)
 * public void repaint (long, int, int, int, int)
 * public void update (Graphics)
 *
 * Each of these methods ensures the panel is constructed
 * and then simply forwards the call to the parent class.
 *
 * You use this class by extending it and moving as much of the
 * constructor code as possible from the child class into the method
 * lazyConstructor.
 */

public abstract class ALazyPanel extends JPanel {

  // We want to call the lazyConstructor only once.
  private boolean lazyConstructorCalled=false;

  public void paint(Graphics g) {
    callLazyConstructor();
    super.paint(g);
  }

  public void paintAll(Graphics g) {
    callLazyConstructor();
    super.paintAll(g);
  }

  public void paintComponents(Graphics g) {
    callLazyConstructor();
    super.paintComponents(g);
  }

  public void repaint() {
    callLazyConstructor();
    super.repaint();
  }

  public void repaint(long l) {
    callLazyConstructor();
    super.repaint(l);
  }

  public void repaint(int i1, int i2, int i3, int i4) {
    callLazyConstructor();
    super.repaint(i1, i2, i3, i4);
  }

  public void repaint(long l, int i1, int i2, int i3, int i4) {
    callLazyConstructor();
    super.repaint(l, i1, i2, i3, i4);
  }

  public void update(Graphics g) {
    callLazyConstructor();
    super.update(g);
  }

  /**
   * Force the lazyConstructor() method implemented in the child class
   * to be called. If this method is called more than once on
   * a given object, all calls but the first do nothing.
   */
  public synchronized final void callLazyConstructor() {
    // The general idea below is as follows:
    // 1) See if this method has already been successfully called.
    // If so, return without doing anything.
    //
    // 2) Otherwise ... call the lazy constructor.
    // 3) Call validate so that any components added are visible.
    // 4) Note that we have run.

    if((lazyConstructorCalled==false)&&(getParent()!=null)) {
      lazyConstructor();
      lazyConstructorCalled=true;
      validate();
    }
  }

  /**
   * This method must be implemented by any child class. Most of
   * the component creation code that would have gone in the constructor
   * of the child goes here instead. See the example
   * at the top.
   */
  abstract protected void lazyConstructor();

}

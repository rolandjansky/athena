package atlantis.graphics;

import java.awt.*;
import java.awt.image.*;
import java.awt.image.renderable.*;
import java.awt.geom.*;
import java.awt.font.*;

/**
 * This graphics is used when nothing should really be drawn on the screen
 */

public class ADummyGraphics2D extends Graphics2D {

  private int x;
  private int y;
  private int width;
  private int height;

  public ADummyGraphics2D() {}

  public Rectangle getClipBounds() {
    return new Rectangle(x, y, width, height);
  }

  public void setClip(int x, int y, int width, int height) {
    this.x=x;
    this.y=y;
    this.width=width;
    this.height=height;
  }

  public void transform(AffineTransform z) {}

  public void fill(Shape z) {}

  public void draw(Shape z) {}

  public void drawImage(BufferedImage a, BufferedImageOp b, int c, int z) {}

  public boolean drawImage(Image a, AffineTransform b, ImageObserver z) {
    return false;
  }

  public void drawRenderedImage(RenderedImage a, AffineTransform z) {}

  public void drawRenderableImage(RenderableImage a, AffineTransform z) {}

  public void drawString(java.text.AttributedCharacterIterator a, float b, float z) {}

  public void drawString(String a, int b, int z) {}

  public void drawString(String a, float b, float z) {}

  public void drawString(java.text.AttributedCharacterIterator a, int b, int z) {}

  public void drawGlyphVector(GlyphVector a, float b, float z) {}

  public boolean hit(Rectangle a, Shape b, boolean z) {
    return false;
  }

  public GraphicsConfiguration getDeviceConfiguration() {
    return null;
  }

  public void setComposite(Composite z) {}

  public void setPaint(Paint z) {}

  public void setStroke(Stroke z) {}

  public void setRenderingHint(RenderingHints.Key a, Object z) {}

  public Object getRenderingHint(RenderingHints.Key z) {
    return null;
  }

  public void setRenderingHints(java.util.Map z) {}

  public void addRenderingHints(java.util.Map z) {}

  public RenderingHints getRenderingHints() {
    return null;
  }

  public void translate(double a, double z) {}

  public void translate(int a, int z) {}

  public void rotate(double a, double b, double z) {}

  public void rotate(double z) {}

  public void scale(double a, double z) {}

  public void shear(double a, double z) {}

  public void setTransform(AffineTransform z) {}

  public AffineTransform getTransform() {
    return null;
  }

  public Paint getPaint() {
    return null;
  }

  public Composite getComposite() {
    return null;
  }

  public void setBackground(Color z) {}

  public Color getBackground() {
    return null;
  }

  public Stroke getStroke() {
    return null;
  }

  public void clip(Shape z) {}

  public FontRenderContext getFontRenderContext() {
    return null;
  }

  public void setColor(Color z) {}

  public Graphics create() {
    return null;
  }

  public boolean drawImage(Image a, int b, int c, int d, int e, int f, int g, int h, int i, ImageObserver z) {
    return false;
  }

  public boolean drawImage(Image a, int b, int c, ImageObserver z) {
    return false;
  }

  public boolean drawImage(Image a, int b, int c, Color d, ImageObserver z) {
    return false;
  }

  public boolean drawImage(Image a, int b, int c, int d, int e, Color f, ImageObserver z) {
    return false;
  }

  public boolean drawImage(Image a, int b, int c, int d, int e, ImageObserver z) {
    return false;
  }

  public boolean drawImage(Image a, int b, int c, int d, int e, int f, int g, int h, int i, Color j, ImageObserver z) {
    return false;
  }

  public Color getColor() {
    return null;
  }

  public void fillRect(int a, int b, int c, int z) {}

  public void setPaintMode() {}

  public void setXORMode(Color z) {}

  public Font getFont() {
    return null;
  }

  public void setFont(Font z) {}

  public FontMetrics getFontMetrics(Font z) {
    return null;
  }

  // public  Rectangle getClipBounds () {  return null;}
  public void clipRect(int a, int b, int c, int z) {}

  // public  void setClip(int a,int b,int c,int z) {  }
  public void setClip(Shape z) {}

  public Shape getClip() {
    return null;
  }

  public void copyArea(int a, int b, int c, int d, int e, int z) {}

  public void drawLine(int a, int b, int c, int z) {}

  public void clearRect(int a, int b, int c, int z) {}

  public void drawRoundRect(int a, int b, int c, int d, int e, int z) {}

  public void fillRoundRect(int a, int b, int c, int d, int e, int z) {}

  public void drawOval(int a, int b, int c, int z) {}

  public void fillOval(int a, int b, int c, int z) {}

  public void drawArc(int a, int b, int c, int d, int e, int z) {}

  public void fillArc(int a, int b, int c, int d, int e, int z) {}

  public void drawPolyline(int[] a, int[] b, int z) {}

  public void drawPolygon(int[] a, int[] b, int z) {}

  public void fillPolygon(int[] a, int[] b, int z) {}

  public void dispose() {}

}

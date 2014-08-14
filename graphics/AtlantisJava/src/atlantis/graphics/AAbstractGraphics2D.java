package atlantis.graphics;

import java.awt.*;
import java.awt.font.*;
import java.awt.geom.*;
import java.awt.image.*;
import java.awt.image.renderable.*;

/**
 * This graphics is used when nothing should really be drawn on the screen
 */

public abstract class AAbstractGraphics2D extends Graphics2D {

  public void transform(AffineTransform z) {
    if(true) throw new RuntimeException();
  }

  public void fill(Shape z) {
    if(true) throw new RuntimeException();
  }

  public void draw(Shape z) {
    if(true) throw new RuntimeException();
  }

  public void drawImage(BufferedImage a, BufferedImageOp b, int c, int z) {
    if(true) throw new RuntimeException();
  }

  public boolean drawImage(Image a, AffineTransform b, ImageObserver z) {
    if(true) throw new RuntimeException();
    return false;
  }

  public void drawRenderedImage(RenderedImage a, AffineTransform z) {
    if(true) throw new RuntimeException();
  }

  public void drawRenderableImage(RenderableImage a, AffineTransform z) {
    if(true) throw new RuntimeException();
  }

  public void drawString(java.text.AttributedCharacterIterator a, float b, float z) {
    if(true) throw new RuntimeException();
  }

  public void drawString(String a, int b, int z) {
    if(true) throw new RuntimeException();
  }

  public void drawString(String a, float b, float z) {
    if(true) throw new RuntimeException();
  }

  public void drawString(java.text.AttributedCharacterIterator a, int b, int z) {
    if(true) throw new RuntimeException();
  }

  public void drawGlyphVector(GlyphVector a, float b, float z) {
    if(true) throw new RuntimeException();
  }

  public boolean hit(Rectangle a, Shape b, boolean z) {
    if(true) throw new RuntimeException();
    return false;
  }

  public GraphicsConfiguration getDeviceConfiguration() {
    if(true) throw new RuntimeException();
    return null;
  }

  public void setComposite(Composite z) {
    if(true) throw new RuntimeException();
  }

  public void setPaint(Paint z) {
    if(true) throw new RuntimeException();
  }

  public void setStroke(Stroke z) {
    if(true) throw new RuntimeException();
  }

  public void setRenderingHint(RenderingHints.Key a, Object z) {
    if(true) throw new RuntimeException();
  }

  public Object getRenderingHint(RenderingHints.Key z) {
    if(true) throw new RuntimeException();
    return null;
  }

  public void setRenderingHints(java.util.Map z) {
    if(true) throw new RuntimeException();
  }

  public void addRenderingHints(java.util.Map z) {
    if(true) throw new RuntimeException();
  }

  public RenderingHints getRenderingHints() {
    if(true) throw new RuntimeException();
    return null;
  }

  public void translate(double a, double z) {
    if(true) throw new RuntimeException();
  }

  public void translate(int a, int z) {
    if(true) throw new RuntimeException();
  }

  public void rotate(double a, double b, double z) {
    if(true) throw new RuntimeException();
  }

  public void rotate(double z) {
    if(true) throw new RuntimeException();
  }

  public void scale(double a, double z) {
    if(true) throw new RuntimeException();
  }

  public void shear(double a, double z) {
    if(true) throw new RuntimeException();
  }

  public void setTransform(AffineTransform z) {
    if(true) throw new RuntimeException();
  }

  public AffineTransform getTransform() {
    if(true) throw new RuntimeException();
    return null;
  }

  public Paint getPaint() {
    if(true) throw new RuntimeException();
    return null;
  }

  public Composite getComposite() {
    if(true) throw new RuntimeException();
    return null;
  }

  public void setBackground(Color z) {
    if(true) throw new RuntimeException();
  }

  public Color getBackground() {
    if(true) throw new RuntimeException();
    return null;
  }

  public Stroke getStroke() {
    if(true) throw new RuntimeException();
    return null;
  }

  public void clip(Shape z) {
    if(true) throw new RuntimeException();
  }

  public FontRenderContext getFontRenderContext() {
    if(true) throw new RuntimeException();
    return null;
  }

  public void setColor(Color z) {
    if(true) throw new RuntimeException();
  }

  public Graphics create() {
    if(true) throw new RuntimeException();
    return null;
  }

  public boolean drawImage(Image a, int b, int c, int d, int e, int f, int g, int h, int i, ImageObserver z) {
    if(true) throw new RuntimeException();
    return false;
  }

  public boolean drawImage(Image a, int b, int c, ImageObserver z) {
    if(true) throw new RuntimeException();
    return false;
  }

  public boolean drawImage(Image a, int b, int c, Color d, ImageObserver z) {
    if(true) throw new RuntimeException();
    return false;
  }

  public boolean drawImage(Image a, int b, int c, int d, int e, Color f, ImageObserver z) {
    if(true) throw new RuntimeException();
    return false;
  }

  public boolean drawImage(Image a, int b, int c, int d, int e, ImageObserver z) {
    if(true) throw new RuntimeException();
    return false;
  }

  public boolean drawImage(Image a, int b, int c, int d, int e, int f, int g, int h, int i, Color j, ImageObserver z) {
    if(true) throw new RuntimeException();
    return false;
  }

  public Color getColor() {
    if(true) throw new RuntimeException();
    return null;
  }

  public void fillRect(int a, int b, int c, int z) {
    if(true) throw new RuntimeException();
  }

  public void setPaintMode() {
    if(true) throw new RuntimeException();
  }

  public void setXORMode(Color z) {
    if(true) throw new RuntimeException();
  }

  public Font getFont() {
    if(true) throw new RuntimeException();
    return null;
  }

  public void setFont(Font z) {
    if(true) throw new RuntimeException();
  }

  public FontMetrics getFontMetrics(Font z) {
    if(true) throw new RuntimeException();
    return null;
  }

  public Rectangle getClipBounds() {
    if(true) throw new RuntimeException();
    return null;
  }

  public void clipRect(int a, int b, int c, int z) {
    if(true) throw new RuntimeException();
  }

  public void setClip(int a, int b, int c, int z) {
    if(true) throw new RuntimeException();
  }

  public void setClip(Shape z) {
    if(true) throw new RuntimeException();
  }

  public Shape getClip() {
    if(true) throw new RuntimeException();
    return null;
  }

  public void copyArea(int a, int b, int c, int d, int e, int z) {
    if(true) throw new RuntimeException();
  }

  // public  void drawLine(int a,int b,int c,int z) { if (true) throw new RuntimeException(); }
  public void clearRect(int a, int b, int c, int z) {
    if(true) throw new RuntimeException();
  }

  public void drawRoundRect(int a, int b, int c, int d, int e, int z) {
    if(true) throw new RuntimeException();
  }

  public void fillRoundRect(int a, int b, int c, int d, int e, int z) {
    if(true) throw new RuntimeException();
  }

  public void drawOval(int a, int b, int c, int z) {
    if(true) throw new RuntimeException();
  }

  public void fillOval(int a, int b, int c, int z) {
    if(true) throw new RuntimeException();
  }

  public void drawArc(int a, int b, int c, int d, int e, int z) {
    if(true) throw new RuntimeException();
  }

  public void fillArc(int a, int b, int c, int d, int e, int z) {
    if(true) throw new RuntimeException();
  }

  public void drawPolyline(int[] a, int[] b, int z) {
    if(true) throw new RuntimeException();
  }

  public void drawPolygon(int[] a, int[] b, int z) {
    if(true) throw new RuntimeException();
  }

  public void fillPolygon(int[] a, int[] b, int z) {
    if(true) throw new RuntimeException();
  }

  public void dispose() {
    if(true) throw new RuntimeException();
  }

}

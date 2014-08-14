package atlantis.canvas;

import atlantis.graphics.AAbstractGraphics2D;
import com.sun.opengl.util.BufferUtil;
import java.awt.*;
import java.awt.RenderingHints.Key;
import java.awt.color.ColorSpace;
import java.awt.geom.AffineTransform;
import java.awt.geom.PathIterator;
import java.awt.geom.Rectangle2D;
import java.awt.image.BufferedImage;
import java.awt.image.ComponentColorModel;
import java.awt.image.DataBuffer;
import java.awt.image.DataBufferByte;
import java.awt.image.ImageObserver;
import java.awt.image.Raster;
import java.awt.image.WritableRaster;
import java.nio.ByteBuffer;
import java.nio.IntBuffer;
import java.text.AttributedCharacterIterator;
import javax.media.opengl.*;

/**
 *
 * This class is an implementation of Graphics2D (although it actually subclasses
 * AAbstractGraphics2D). It wraps a GLAutoDrawable object to allow Swing based
 * components to paint and have their calls translated into OpenGL calls on the
 * underlying context.
 * 
 * Obviously emulating all the functionality of Graphics2D is a challenge
 * so expect some visual issues initially...
 * 
 * @author Adam Davison
 */
public class AGLGraphics extends AAbstractGraphics2D {

    private int m_clipx = 0;
    private int m_clipy = 0;
    private int m_clipw = 0;
    private int m_cliph = 0;
    private Color m_color = Color.BLACK;
    private GLAutoDrawable m_d;
    private GL m_gl;
    private int m_depth = 1;
    private ComponentColorModel m_colorModel =
            new ComponentColorModel(ColorSpace.getInstance(ColorSpace.CS_sRGB),
            new int[]{8, 8, 8, 8}, true, false, ComponentColorModel.TRANSLUCENT,
            DataBuffer.TYPE_BYTE);

    public AGLGraphics(AGLGraphics old) {
        m_d = old.m_d;
        m_gl = m_d.getGL();
    }

    /*
     * Draw shapes, don't actually implement cubic or quadratic curves yet.
     * For now we only draw straight lines.
     * Having said that Atlantis uses such a huge number of segments that this
     * doesn't seem to be an issue for visual quality as of yet...
     */
    @Override
    public void draw(Shape z) {
        PathIterator pi = z.getPathIterator(null);
        float[] c = new float[6];
        float xpos = 0.0f;
        float ypos = 0.0f;
        while (!pi.isDone()) {
            int type = pi.currentSegment(c);
            switch (type) {
                case PathIterator.SEG_MOVETO:
                    xpos = c[0];
                    ypos = c[1];
                    break;
                case PathIterator.SEG_CLOSE:
                    // Ignoring close lines for now I guess...
                    break;
                case PathIterator.SEG_CUBICTO:
                    drawLine(c[2], c[3], c[4], c[5]);
                case PathIterator.SEG_QUADTO:
                    drawLine(c[0], c[1], c[2], c[3]);
                case PathIterator.SEG_LINETO:
                    // Just do lines...
                    drawLine(xpos, ypos, c[0], c[1]);
                    xpos = c[0];
                    ypos = c[1];
                    break;
            }
            pi.next();
        }
    //super.draw(z);
    }

    public AGLGraphics(GLAutoDrawable d) {
        m_d = d;
        m_gl = m_d.getGL();
    }

    /*
     * This implementation of create()/dispose() is highly thread unsafe
     * Since it's not trivial to duplicate the GL context we do the best we can
     * and hopefully it's ok for Atlantis.
     * 
     * Essentially where Java2D would create a whole new object which would then
     * have it's own state, we push the modelview stack. This means that
     * we can perform transformations and then pop the modelview stack in dispose
     * to emulate this functionality.
     * 
     * Obviously if you call create twice in two separate threads then one of
     * you is destroying the other one's modelview matrix...
     */
    @Override
    public Graphics create() {
        m_gl.glMatrixMode(GL.GL_MODELVIEW_MATRIX);
        m_gl.glPushMatrix();
        m_depth++;
        //System.err.println("PUSH: " + depth);
        //return new AGLGraphics(this); //?????
        return this;
    }

    @Override
    public void translate(int x, int y) {
        m_gl.glMatrixMode(GL.GL_MODELVIEW_MATRIX);
        m_gl.glTranslatef(x, y, 0.0f);
        return;
    }

    @Override
    public Color getColor() {
        return m_color;
    }

    @Override
    public void setColor(Color c) {
        m_color = c;
        //System.out.println(c.toString());
        float r = ((float) c.getRed()) / 255.0f;
        float g = ((float) c.getGreen()) / 255.0f;
        float b = ((float) c.getBlue()) / 255.0f;
        float a = ((float) c.getAlpha()) / 255.0f;
        m_d.getGL().glColor4f(r, g, b, a);
    }

    @Override
    public void setPaintMode() {
        throw new UnsupportedOperationException("Not supported yet.");
    }

    @Override
    public void setXORMode(Color c1) {
        throw new UnsupportedOperationException("Not supported yet.");
    }
    
    private Font m_font = Font.decode("Arial-BOLD-18");

    @Override
    public Font getFont() {
        return m_font;
    }

    @Override
    public void setFont(Font font) {
        m_font = font;
    }

    @Override
    public FontMetrics getFontMetrics(Font f) {
        throw new UnsupportedOperationException("Not supported yet.");
    }

    @Override
    public Rectangle getClipBounds() {
        return new Rectangle(m_clipx, m_clipy, m_clipw, m_cliph);
    }

    @Override
    public void clipRect(int x, int y, int width, int height) {
        throw new UnsupportedOperationException("Not supported yet.");
    }

    /*
     * Sort of ignoring clip here...
     */
    @Override
    public void setClip(int x, int y, int width, int height) {
        m_clipx = x;
        m_clipy = y;
        m_clipw = width;
        m_cliph = height;
    //System.out.println("Who cares about clip for now??");
    //throw new UnsupportedOperationException("Not supported yet.");
    }

    @Override
    public Shape getClip() {
        throw new UnsupportedOperationException("Not supported yet.");
    }

    @Override
    public void setClip(Shape clip) {
        throw new UnsupportedOperationException("Not supported yet.");
    }

    @Override
    public void copyArea(int x, int y, int width, int height, int dx, int dy) {
        throw new UnsupportedOperationException("Not supported yet.");
    }

    @Override
    public void drawLine(int x1, int y1, int x2, int y2) {
        drawLine((float) x1, (float) y1, (float) x2, (float) y2);
    }

    public void drawLine(float x1, float y1, float x2, float y2) {
        m_gl.glBegin(m_gl.GL_LINES);
        m_gl.glVertex2f(x1, y1);
        m_gl.glVertex2f(x2, y2);
        m_gl.glEnd();
    }

    @Override
    public void fillRect(int x, int y, int width, int height) {
        GL gl = m_d.getGL();

        gl.glPolygonMode(gl.GL_FRONT_AND_BACK, gl.GL_FILL);

        gl.glBegin(gl.GL_QUADS);
        gl.glVertex2f(x, y);
        gl.glVertex2f(x + width, y);
        gl.glVertex2f(x + width, y + height);
        gl.glVertex2f(x, y + height);
        gl.glEnd();
    }

    @Override
    public void clearRect(int x, int y, int width, int height) {
        throw new UnsupportedOperationException("Not supported yet.");
    }

    @Override
    public void drawRoundRect(int x, int y, int width, int height, int arcWidth, int arcHeight) {
        throw new UnsupportedOperationException("Not supported yet.");
    }

    @Override
    public void fillRoundRect(int x, int y, int width, int height, int arcWidth, int arcHeight) {
        throw new UnsupportedOperationException("Not supported yet.");
    }

    @Override
    public void drawOval(int x, int y, int width, int height) {
        throw new UnsupportedOperationException("Not supported yet.");

    }

    @Override
    public void fillOval(int x, int y, int width, int height) {
        //throw new UnsupportedOperationException("Not supported yet.");
        return;
    }

    @Override
    public void drawArc(int x, int y, int width, int height, int startAngle, int arcAngle) {
        throw new UnsupportedOperationException("Not supported yet.");
    }

    @Override
    public void fillArc(int x, int y, int width, int height, int startAngle, int arcAngle) {
        throw new UnsupportedOperationException("Not supported yet.");
    }

    @Override
    public void drawPolyline(int[] xPoints, int[] yPoints, int nPoints) {
        throw new UnsupportedOperationException("Not supported yet.");
    }

    @Override
    public void drawPolygon(int[] xPoints, int[] yPoints, int nPoints) {
        m_gl.glPolygonMode(m_gl.GL_FRONT_AND_BACK, m_gl.GL_LINE);

        m_gl.glBegin(m_gl.GL_POLYGON);

        for (int i = 0; i < nPoints; i++) {
            m_gl.glVertex2f(xPoints[i], yPoints[i]);
        }

        m_gl.glEnd();
    }

    public void fillPolygon(double[] xPoints, double[] yPoints, int nPoints) {

        m_gl.glPolygonMode(m_gl.GL_FRONT_AND_BACK, m_gl.GL_FILL);

        m_gl.glBegin(m_gl.GL_POLYGON);

        for (int i = 0; i < nPoints; i++) {
            m_gl.glVertex2d(xPoints[i], yPoints[i]);
        }

        m_gl.glEnd();
    }

    /*
     * Bear in mind that although Java2D is capable of filling arbitrary
     * polygons, OpenGL/graphics cards in general use a very simple fill
     * algorithm which is only guaranteed to be valid for convex polygons
     * If you want to render a concave polygon you have to tessellate it down
     * into convex ones.
     * 
     * So far I've been able to do this by modifying the way geometry is
     * represented but you can also have a go at tessellating automatically
     * using GLUT if you really need to...
     */
    @Override
    public void fillPolygon(int[] xPoints, int[] yPoints, int nPoints) {
        
        // Prototype code for vertex arrays, actually slower for single polys
        // but will definitely improve importance if we start passing in big
        // arrays of polygons, say for complex geometry
        if (false) {
            IntBuffer ib = BufferUtil.newIntBuffer(nPoints * 2);
            for (int i = 0; i < nPoints; i++) {
                ib.put(xPoints[i]);
                ib.put(yPoints[i]);
            }
            ib.rewind();
            m_gl.glEnableClientState(GL.GL_VERTEX_ARRAY);
            m_gl.glVertexPointer(2, GL.GL_INT, 0, ib);
            m_gl.glDrawArrays(GL.GL_POLYGON, 0, nPoints);
        } else {
            // Draw the simple way
            
            m_gl.glPolygonMode(m_gl.GL_FRONT_AND_BACK, m_gl.GL_FILL);
            m_gl.glBegin(m_gl.GL_POLYGON);

            for (int i = 0; i < nPoints; i++) {
                m_gl.glVertex2f(xPoints[i], yPoints[i]);
            }

            m_gl.glEnd();
        }
    }

    @Override
    public Stroke getStroke() {
        return null;
    }

    @Override
    public void setStroke(Stroke z) {
    //super.setStroke(z);
    }

    @Override
    public void drawString(String str, int x, int y) {
        //FontRenderContext frc = new FontRenderContext(null, true, true);
        //GlyphVector gv = m_font.layoutGlyphVector(frc, str.toCharArray(),
        //        0, str.length(), Font.LAYOUT_LEFT_TO_RIGHT);
        //int ng = gv.getNumGlyphs();
        //Rectangle2D r = gv.getVisualBounds();
        //m_gl.glColor3f(1.0f, 0.0f, 0.0f);
        //this.fillRect((int) r.getX(), (int) r.getY(), (int) r.getWidth(), (int) r.getHeight());
        //this.setColor(m_color);
                
        WritableRaster test = Raster.createInterleavedRaster(DataBuffer.TYPE_BYTE,
                32, 32, 4, null);
        BufferedImage testi = new BufferedImage(m_colorModel, test, false, null);
        Graphics testg = testi.getGraphics();
        FontMetrics fm = testg.getFontMetrics(m_font);
        Rectangle2D r = fm.getStringBounds(str, testg);
                
        WritableRaster raster =
                Raster.createInterleavedRaster(DataBuffer.TYPE_BYTE,
                (int)r.getWidth(), (int)r.getHeight(), 4, null);

        BufferedImage bi = new BufferedImage(m_colorModel, raster, false, null);
        
        Graphics big = bi.getGraphics();
        //big.setColor(Color.GREEN);
        //big.fillRect(0, 0, bi.getWidth(), bi.getHeight());
        big.setColor(m_color);
        big.setFont(m_font);
        big.drawString(str, -(int)r.getX(), -(int)r.getY());
        drawImage(bi, x, (int)(y - r.getHeight()), null);
    }

    @Override
    public void drawString(AttributedCharacterIterator iterator, int x, int y) {
    //throw new UnsupportedOperationException("Not supported yet.");
    }

    @Override
    public boolean drawImage(Image img, int x, int y, ImageObserver observer) {
        WritableRaster raster =
                Raster.createInterleavedRaster(DataBuffer.TYPE_BYTE,
                img.getWidth(observer), img.getHeight(observer), 4, null);

        BufferedImage bi = new BufferedImage(m_colorModel, raster, false, null);

        Graphics2D g = bi.createGraphics();
        //g.setColor(Color.WHITE);
        //g.drawLine(0, 0, bi.getWidth(), bi.getHeight());
        AffineTransform gt = new AffineTransform();
        gt.translate(0, img.getHeight(observer));
        gt.scale(1, -1d);
        g.transform(gt);
        g.drawImage(img, null, null);

        DataBufferByte imgbuf = (DataBufferByte) raster.getDataBuffer();

        m_gl.glRasterPos2i(x, y + bi.getHeight());
        //System.out.println(img.getWidth(observer) + ":" + img.getHeight(observer));
        m_gl.glDrawPixels(img.getWidth(observer), img.getHeight(observer),
                m_gl.GL_RGBA, m_gl.GL_UNSIGNED_BYTE, ByteBuffer.wrap(imgbuf.getData()));

        //System.out.println("Ignoring drawImage for now...");
        return true;
    //throw new UnsupportedOperationException("Not supported yet.");
    }

    @Override
    public boolean drawImage(Image img, int x, int y, int width, int height, ImageObserver observer) {
        throw new UnsupportedOperationException("Not supported yet.");
    }

    @Override
    public boolean drawImage(Image img, int x, int y, Color bgcolor, ImageObserver observer) {
        throw new UnsupportedOperationException("Not supported yet.");
    }

    @Override
    public boolean drawImage(Image img, int x, int y, int width, int height, Color bgcolor, ImageObserver observer) {
        throw new UnsupportedOperationException("Not supported yet.");
    }

    @Override
    public boolean drawImage(Image img, int dx1, int dy1, int dx2, int dy2, int sx1, int sy1, int sx2, int sy2, ImageObserver observer) {
        // Needed to render JComponent backgrounds
        //throw new UnsupportedOperationException("Not supported yet.");
        return true;
    }

    @Override
    public Object getRenderingHint(Key z) {
        return null;
    }

    @Override
    public void setRenderingHint(Key a, Object z) {
        return;
    }

    @Override
    public boolean drawImage(Image img, int dx1, int dy1, int dx2, int dy2, int sx1, int sy1, int sx2, int sy2, Color bgcolor, ImageObserver observer) {
        throw new UnsupportedOperationException("Not supported yet.");
    }

    @Override
    public void dispose() {
        m_depth--;
        if (m_depth == 0) {
            m_depth = 1;
        // This happens when the object is really finally destroyed...
        // When this is actually happening if we pop we'll cause a crash...
        //System.err.println("DISPOSE CALLED TWICE ON ONE OBJECT!!!");
        //(new Exception()).printStackTrace();
        } else {
            m_gl.glMatrixMode(GL.GL_MODELVIEW_MATRIX);
            m_gl.glPopMatrix();
        }
        return;
    // We didn't create so no need to dispose... probably bad...
    //throw new UnsupportedOperationException("Not supported yet.");
    }

    @Override
    public void rotate(double a, double b, double z) {
        super.rotate(a, b, z);
    }

    @Override
    public void rotate(double z) {
        m_gl.glMatrixMode(GL.GL_MODELVIEW_MATRIX);
        m_gl.glRotated(z * 360 / (2 * Math.PI), 0.0, 0.0, 1.0);
    }
}
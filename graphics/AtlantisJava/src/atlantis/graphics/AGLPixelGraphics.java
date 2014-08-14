package atlantis.graphics;

import atlantis.canvas.AGLGraphics;

/*
 * This class subclasses APixelGraphics to allow some optimisations slightly
 * higher up the graphics chain in cases where OpenGL renders faster in
 * certain ways
 * 
 * @author Adam Davison
 */

public class AGLPixelGraphics extends APixelGraphics {

    protected AGLGraphics m_glg;

    public AGLPixelGraphics(AGLGraphics g) {
        super(g);
        m_glg = g;
    }

    @Override
    public void fillPolygon(double[] h, double[] v, int numPoints) {
        /*int type = getContainmentType(h, v, numPoints, POLYGON);
        if (type == FULLY_INSIDE || type == INTERSECTS) {
            if (type == INTERSECTS) {
                ACoord clipped = clipper.clipPolygon(h, v, numPoints);
                h = clipped.hv[0][0];
                v = clipped.hv[1][0];
                numPoints = h.length;
            }*/
            /*int[] hInt = new int[numPoints];
            int[] vInt = new int[numPoints];
            for (int i = 0; i < numPoints; ++i) {
                hInt[i] = (int) h[i];
                vInt[i] = (int) v[i];
            }*/
            updateColor();
            m_glg.fillPolygon(h, v, numPoints);
            //g.drawPolygon(hInt, vInt, numPoints);
        /*} else if (type == CONTAINS) {
            Rectangle bounds = g.getClipBounds();
            updateColor();
            g.fillRect((int) Math.rint(bounds.getX()), (int) Math.rint(bounds.getY()), (int) Math.rint(bounds.getWidth()), (int) Math.rint(bounds.getHeight()));
        }*/
    }
}

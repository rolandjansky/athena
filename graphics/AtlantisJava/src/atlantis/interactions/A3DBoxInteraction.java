package atlantis.interactions;

import java.awt.Color;
import java.awt.Graphics2D;
import java.awt.event.KeyEvent;
import java.awt.geom.Ellipse2D;
import java.awt.geom.Point2D;

import javax.swing.JMenuItem;

import atlantis.parameters.APar;
import atlantis.parameters.AParameter;
import atlantis.projection.AProjection;
import atlantis.projection.AProjection3DBox;
import atlantis.graphics.ACursorFactory;

/**
 * The Zoom and Rotate interaction.
 */
public class A3DBoxInteraction extends AInteraction implements
        ASleepMouseDragListener, AMousePressListener, AEnterExitListener
{
    private static final int ZOOM_MODE = 0;
    private static final int HORIZONTAL_ZOOM_MODE = 1;
    private static final int VERTICAL_ZOOM_MODE = 2;
    private static final int ROTATE_MODE = 3;

    private int d = 10;
    private Point2D.Double p0;
    private int previousKey = KeyEvent.VK_UNDEFINED;
    private int mode;

    public A3DBoxInteraction()
    {
        super(1, AUTOMATIC_REPAINT, SCREEN_COORDINATES, false);

        // The center of the ellipse is in (0, 0) in User Space
        // dummy for now
        hr[0] = new Ellipse2D.Double(-d / 2, -d / 2, d, d);
    }

    public void connect(AInteractionsManager manager)
    {
        super.connect(manager);
    }

    public int getPressButton()
    {
        return AInteractionsManager.LEFT;
    }

    public int init(Point2D.Double p, int key)
    {
        keyChange(p, key);
        return -1;
    }

    public void pressed(Point2D.Double p, int button, int key)
    {}

    public int getButton()
    {
        return AInteractionsManager.LEFT;
    }

    public void start(Point2D.Double p, int region, int key)
    {}

    private void keyChange(Point2D.Double p, int key)
    {
        p0 = p;

        switch (key)
        {
            case KeyEvent.VK_UNDEFINED:
                mode = ZOOM_MODE;
                break;

            case KeyEvent.VK_H:
                mode = HORIZONTAL_ZOOM_MODE;
                break;

            case KeyEvent.VK_V:
                mode = VERTICAL_ZOOM_MODE;
                break;

            case KeyEvent.VK_R:
                mode = ROTATE_MODE;
                break;

            case KeyEvent.VK_Z:
                mode = ZOOM_MODE;
                break;
        }
    }

    public void drag(Point2D.Double p, int region, int key)
    {
        if (key != previousKey)
        {
            keyChange(p, key);
            previousKey = key;
        }

        double zf;
        AProjection proj = window.getProjection();
        Point2D.Double center = ((AProjection3DBox) proj).getCenter(window);

        switch (mode)
        {
            case ZOOM_MODE:
                zf = p0.distance(center.getX(), center.getY())
                        / p.distance(center.getX(), center.getY());
                ((AProjection3DBox) proj).zoom(zf);
                p0 = p;
                window.repaintFromScratch();
                break;
            case HORIZONTAL_ZOOM_MODE:
                zf = Math.abs((p0.getX() - center.getX())
                        / (p.getX() - center.getX()));
                ((AProjection3DBox) proj).zoomHorizontal(zf);
                p0 = p;
                window.repaintFromScratch();
                break;
            case VERTICAL_ZOOM_MODE:
                zf = Math.abs((p0.getY() - center.getY())
                        / (p.getY() - center.getY()));
                ((AProjection3DBox) proj).zoomVertical(zf);
                p0 = p;
                window.repaintFromScratch();
                break;
            case ROTATE_MODE:
                AParameter phiPar = parameterStore.get(proj.getName(), "Phi");
                double height = window.getCurrDisp().getHeight();
                double deltaV = p.getY() - p0.getY();
                phiPar.setD(adjustPhi(phiPar.getD() + 360. * deltaV / height));
                p0 = p;
                window.repaintFromScratch();
                break;
        }
    }

    private double adjustPhi(double phi)
    {
        while (phi < 0)
            phi += 360;
        while (phi > 360)
            phi -= 360;
        return phi;
    }

    public void stop()
    {}

    public void cancel()
    {}

    public void paint(Graphics2D g)
    {
        Point2D.Double p = ((AProjection3DBox) window.getProjection()).getCenter(window);
        g.setColor(Color.red);
        g.fillOval((int) (p.x - d / 2), (int) (p.y - d / 2), d, d);
    }

    public void entered()
    {
//        if (region == 0)
            window.setCursor(ACursorFactory.getInstance().getDefaultCursor());
    }

    public void exited()
    {
 //       if (region == 0)
            window.setCursor(ACursorFactory.getInstance().getDefaultCursor());
    }

    public int getPopupType()
    {
        return APopupListener.WINDOW_POPUP;
    }

    /*public void showPopup(Point p, int region)
    {
        popupMenu.show(window, p.x, p.y);
    }*/
    
    public JMenuItem[] getPopupItems() {
        return null;
    }

    public AModifier[] getModifiers()
    {
        return new AModifier[] {
            new AModifier(KeyEvent.VK_UNDEFINED, false, "Change the volume of the box"),
            new AModifier(KeyEvent.VK_Z, false, "Change the volume of the box"),
            new AModifier(KeyEvent.VK_H, false, "Change the length of the box"),
            new AModifier(KeyEvent.VK_V, false, "Change the height of the box"),
            new AModifier(KeyEvent.VK_R, false, "Rotate around axis of box")
        };
    }
    
}

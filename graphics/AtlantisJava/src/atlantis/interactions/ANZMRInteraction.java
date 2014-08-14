package atlantis.interactions;

import atlantis.nge.ANManager;
import atlantis.nge.ANPickDrawer;
import atlantis.nge.ANPickResultPrinter;
import atlantis.nge.ANProjection;
import atlantis.nge.ANProjection3D;
import atlantis.nge.ANProjectionPhysics;
import java.awt.Graphics2D;
import java.awt.geom.Point2D.Double;

/**
 *
 * @author Adam Davison, Mark Stockton
 */
public class ANZMRInteraction extends AInteraction implements ASleepMouseDragListener, AMousePressListener {

    private Double m_last = null;
    
    public ANZMRInteraction() {
        super(0, WINDOW_TOTAL_REPAINT, SCREEN_COORDINATES, false);
    }
    
    @Override
    public void paint(Graphics2D g) {
        //throw new UnsupportedOperationException("Not supported yet.");
    }

    public void doRotation(double dx, double dy) {
        ANProjection np = (ANManager.getManager().getFrameManager(window).getTargetProjection());
        if (np instanceof ANProjection3D) {
            ((ANProjection3D) np).dPhi((float) (dx / 4.0));
            ((ANProjection3D) np).dElev((float) (dy / 4.0));
        }else if (np instanceof ANProjectionPhysics) {
            ((ANProjectionPhysics) np).dPhi((float) (dx / 4.0));
            ((ANProjectionPhysics) np).dElev((float) (dy / 4.0));
        }
    }
    
    public int getButton() {
        return AInteractionsManager.LEFT;
    }

    public void start(Double p, int region, int key) {
        m_last = p;
    }

    public void drag(Double p, int region, int key) {
        double dx = (p.getX() - m_last.getX());
        double dy = (p.getY() - m_last.getY());
        
        doRotation(dx,dy);
        
        m_last = p;
    }

    public void stop() {
        
    }

    public void cancel() {
        
    }

    public int init(Double p, int key) {
        return -1;
    }

    public int getPressButton() {
        return AInteractionsManager.ALL;
    }

    public void pressed(Double p, int button, int key) {
        System.out.println("ZMR PRESS: " + p.getX() + ":" + p.getY());
        ANPickDrawer pd = new ANPickDrawer(window, window.getWidth(), window.getHeight(),
                (int)p.getX(), (int)p.getY(), new ANPickResultPrinter());
        pd.doPick();
    }

}

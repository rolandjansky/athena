package atlantis.nge;

import atlantis.canvas.AWindow;
import java.util.ArrayList;
import java.util.Iterator;

/**
 *
 * @author Adam Davison
 */
public class ANAnimationManager {

    public static final int INTERP_LINEAR = 1;
    private ArrayList<ANAnimSegment> m_segs;
    private double m_now;
    
    private ANAnimThread m_at;

    public ANAnimationManager(AWindow w) {
        m_segs = new ArrayList<ANAnimSegment>();
        updateNow();
        m_at = new ANAnimThread(this, w);
        new Thread(m_at).start();
    }

    public void updateNow() {
        m_now = ((double) System.currentTimeMillis()) / 1000.0;
    }

    public void scheduleAnimationAbsolute(ANAnimVar v, double t1, double t2, double v1, double v2) {
        m_segs.add(new ANAnimInterpolate(v, t1, t2, v1, v2, INTERP_LINEAR));
        m_at.reschedule();
    }

    public void scheduleAnimation(ANAnimVar v, double t1, double t2, double v1, double v2) {
        updateNow();
        scheduleAnimationAbsolute(v, t1 + m_now, t2 + m_now, v1, v2);
    }

    public void scheduleProjectionChange(ANFrameManager fm, ANProjection p, double t) {
        updateNow();
        m_segs.add(new ANAnimSetProjection(fm, p, t + m_now));
        m_at.reschedule();
    }

    public void animate() {
        updateNow();

        //System.out.println("Animating at: " + m_now);

        Iterator<ANAnimSegment> it = m_segs.listIterator();
        while (it.hasNext()) {
            ANAnimSegment seg = it.next();
            //System.out.println("Seg: " + seg + " start: " + seg.getStart() + " end: " + seg.getEnd());
            if (seg.getStart() < m_now) {
                seg.evaluate(m_now);
            }
            if (seg.getEnd() < m_now) {
                it.remove();
            }
        }
    }

    public double timeToWait() {
        double next = -1.0;
        updateNow();

        //System.out.println("Wait testing at: " + m_now);

        Iterator<ANAnimSegment> it = m_segs.listIterator();
        while (it.hasNext()) {
            ANAnimSegment seg = it.next();
            //System.out.println("Seg: " + seg + " start: " + seg.getStart() + " end: " + seg.getEnd());
            if (seg.getStart() <= next && seg.getEnd() >= next) {
                //System.out.println("In segment, returning 0.0");
                return 0.0;
            } else if (seg.getStart() >= next) {
                double wait = seg.getStart() - m_now;
                if (wait < next || next == -1.0) {
                    if (wait < 0.0) {
                        wait = 0.0;
                    }
                    next = wait;
                }
            }
        }

        return next;
    }
}



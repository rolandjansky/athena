package atlantis.nge;

import atlantis.canvas.AWindow;
import atlantis.utils.ALogger;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 *
 * @author Adam Davison
 */
public class ANAnimThread implements Runnable {

    private static ALogger logger = ALogger.getLogger(ANAnimThread.class);
    private ANAnimationManager m_am;
    private Object m_wait = new Object();
    private AWindow m_w;

    public ANAnimThread(ANAnimationManager am, AWindow w) {
        m_am = am;
        m_w = w;
    }

    public void reschedule() {
        //System.out.println("Rescheduling");
        synchronized (m_wait) {
            m_wait.notifyAll();
        }
        //System.out.println("Rescheduled");
    }

    public void run() {
        while (true) {
            double wait = m_am.timeToWait();

            //System.out.println("Told to wait: " + wait);

            double mintime = 1.0 / 100.0;

            if (wait == -1.0) {
                // wait forever
                synchronized (m_wait) {
                    try {
                        m_wait.wait();
                    } catch (InterruptedException ex) {
                        logger.error("Interrupted in sleep", ex);
                    }
                }
            } else if (wait > mintime) {
                // wait until we're supposed to do something
                synchronized (m_wait) {
                    try {
                        m_wait.wait((long) (wait * 1000));
                    } catch (InterruptedException ex) {
                        logger.error("Interrupted in sleep", ex);
                    }
                }
            } else {
                // do something now
                //System.out.println("Forcing Repaint!");
                m_w.invalidateQuietly();
                //m_w.getView().paintImmediately(0, 0, m_w.getWidth(), m_w.getHeight());
                m_w.repaint();
            }
        }
    }
}

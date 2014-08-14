package atlantis.interactions;

/**
 * Interface for all FishEyeChangeListener classes. Each time the FishEye
 * parameters are changed, all listeners are called with the new paremeters
 * @author sboeser
 */
public interface AFishEyeChangeListener {

  /**
   * This method gets called each time new fisheye parameters are set
   * @param status the new fishEye status
   * @param value the new fishEye value
   */
  abstract public void fishEyeChange(boolean status, double value);  

}

package atlantis.interactions;

/**
 *
 * @author Adam Davison
 */
public class ANZMRGroup extends AInteractionGroup {
  
    private ANZMRInteraction transformInteraction;

  public ANZMRGroup(AInteractionsManager iManager) {
    super(AInteractionGroup.WINDOW_GROUP, iManager);
    transformInteraction=new ANZMRInteraction();
    this.addInteraction(transformInteraction);
  }
}

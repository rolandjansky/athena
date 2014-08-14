package atlantis.interactions;


public class AZMRGroup extends AInteractionGroup {

  private AZMRInteraction transformInteraction;

  public AZMRGroup(AInteractionsManager iManager) {
    super(AInteractionGroup.WINDOW_GROUP, iManager);
    transformInteraction=new AZMRInteraction();
    this.addInteraction(transformInteraction);
  }

}

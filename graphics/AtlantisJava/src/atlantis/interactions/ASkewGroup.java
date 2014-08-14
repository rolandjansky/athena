package atlantis.interactions;


public class ASkewGroup extends AInteractionGroup {

  ASkewInteraction skewInteraction;

  public ASkewGroup(AInteractionsManager iManager) {
    super(AInteractionGroup.WINDOW_GROUP, iManager);
    skewInteraction=new ASkewInteraction();
    this.addInteraction(skewInteraction);
  }

}

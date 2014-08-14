package atlantis.interactions;


public class AScaleCopyGroup extends AInteractionGroup {

  AScaleCopyInteraction scaleCopyInteraction;

  public AScaleCopyGroup(AInteractionsManager iManager) {
    super(AInteractionGroup.CANVAS_GROUP, iManager);
    scaleCopyInteraction=new AScaleCopyInteraction();
    this.addInteraction(scaleCopyInteraction);
  }

}

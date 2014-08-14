package atlantis.utils;


// does not copy input arrays for efficiency
// these arrays must not be modified !

public class AIntHashtable {

  int size;
  int shift;
  int[] key;
  int[] value;
  int[] next;
  int[] first;

  /*
   public AIntHashtable(int size) {
   this.size=size;
   shift=0;
   count = 0;
   key = new int[size];
   value = new int[size];
   next = new int[size];
   first = new int[size];
   for (int i = 0; i < size; ++i)
   first[i] = -1;
   }
   */

  public AIntHashtable(int[] key, int[] value) {
    this.size=key.length;
    this.key=key;
    this.value=value;
    //
    int mask=0;
    for(int i=0; i<size; ++i) {
      mask=mask|key[i];
      if((mask&1)==1) break;
    }
    shift=0;
    for(int i=0; i<size; ++i) {
      if((mask>>i&1)==1) break;
      shift++;
    }
    //
    next=new int[size];
    first=new int[size];
    for(int i=0; i<size; ++i)
      first[i]=-1;
    for(int i=0; i<size; ++i) {
      int index=(key[i]>>shift)%size;
      if(index<0) index=-index;
      this.next[i]=first[index];
      first[index]=i;
    }
  }

  public AIntHashtable(int[] key) {
    this(key, makeIndexArray(key.length));
  }

  private static int[] makeIndexArray(int length) {
    int[] temp=new int[length];
    for(int i=0; i<length; ++i)
      temp[i]=i;
    return temp;
  }

  public int get(int key) {
    if(size<=0) return -1;
    int index=(key>>shift)%size;
    if(index<0) index=-index;
    for(int e=first[index]; e!=-1; e=next[e])
      if(this.key[e]==key)
        return value[e];
    return -1;
  }

}

package atlantis.utils;

import static org.junit.Assert.assertEquals;
import org.junit.Test;

/**
 * Test for the IdHelper range class used to represent 
 * possible values of an identifier field.
 * 
 * @author Eric Jansen
 */
public class AIdRangeTest {
    
    public AIdRangeTest() {}

    @Test
    public void testEmptyRange() throws Exception {
        AIdRange a = new AIdRange();
        AIdRange b = new AIdRange(new int[] {1, 2});
        
        a.add(b);
        assertEquals(a, b);
        
        b.add(a);
        assertEquals(a, b);
    }
    
    @Test
    public void testContainsRange() throws Exception {
        AIdRange a = new AIdRange(1, 3);
        AIdRange b = new AIdRange(new int[] {1, 2, 4});
        AIdRange c = new AIdRange(1, 2);
        
        assert(a.contains(c));
        assert(b.contains(c));
        assert(!a.contains(b));
        assert(!b.contains(a));
    }

    @Test
    public void testContainsValue() throws Exception {
        AIdRange a = new AIdRange(-5, 5);
        AIdRange b = new AIdRange(new int[] {-1, 3, 9});

        assert(!a.contains(-6));
        assert(a.contains(-5));
        assert(a.contains(2));
        assert(a.contains(5));
        assert(!a.contains(6));
        
        assert(b.contains(-1));
        assert(!b.contains(2));
        assert(b.contains(3));
        assert(b.contains(9));
    }
    
    @Test
    public void testAddRange() throws Exception {
        AIdRange a = new AIdRange(new int[] {-1, 0, 1, 2, 5, 6});
        AIdRange b = new AIdRange(new int[] {-1, 1, 2, 6});
        AIdRange c = new AIdRange(new int[] {0, 1, 2, 5});
        
        b.add(c);
        assert(a.equals(b));
        
        AIdRange d = new AIdRange(-5, 5);
        AIdRange e = new AIdRange(new int[] {-5, -4});
        AIdRange f = new AIdRange(-3, 5);
        
        e.add(f);
        assert(d.equals(e));
        
        AIdRange g = new AIdRange(0, 2);
        AIdRange h = new AIdRange(new int[] {0, 1});
        AIdRange i = new AIdRange(new int[] {1, 2});
        
        h.add(i);
        assert(g.equals(h)); // comparing [ 0 -> 2 ] to [ 0 1 2 ]
    }
    
    @Test
    public void testToString() throws Exception {
        AIdRange a = new AIdRange(-2, 15);
        AIdRange b = new AIdRange(new int[] {2, 3, 5});
        AIdRange c = new AIdRange();
        assertEquals("[ -2 -> 15 ]", a.toString());
        assertEquals("[ 2 3 5 ]", b.toString());
        assertEquals("[ ]", c.toString());
    }
}

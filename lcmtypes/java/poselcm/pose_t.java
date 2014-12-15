/* LCM type definition class file
 * This file was automatically generated by lcm-gen
 * DO NOT MODIFY BY HAND!!!!
 */

package poselcm;
 
import java.io.*;
import java.util.*;
import lcm.lcm.*;
 
public final class pose_t implements lcm.lcm.LCMEncodable
{
    public byte robotid;
    public short position[];
    public short orientation[];
    public short velocity;
 
    public pose_t()
    {
        position = new short[3];
        orientation = new short[4];
    }
 
    public static final long LCM_FINGERPRINT;
    public static final long LCM_FINGERPRINT_BASE = 0x0ecda64b9b709dc0L;
 
    static {
        LCM_FINGERPRINT = _hashRecursive(new ArrayList<Class<?>>());
    }
 
    public static long _hashRecursive(ArrayList<Class<?>> classes)
    {
        if (classes.contains(poselcm.pose_t.class))
            return 0L;
 
        classes.add(poselcm.pose_t.class);
        long hash = LCM_FINGERPRINT_BASE
            ;
        classes.remove(classes.size() - 1);
        return (hash<<1) + ((hash>>63)&1);
    }
 
    public void encode(DataOutput outs) throws IOException
    {
        outs.writeLong(LCM_FINGERPRINT);
        _encodeRecursive(outs);
    }
 
    public void _encodeRecursive(DataOutput outs) throws IOException
    {
        outs.writeByte(this.robotid); 
 
        for (int a = 0; a < 3; a++) {
            outs.writeShort(this.position[a]); 
        }
 
        for (int a = 0; a < 4; a++) {
            outs.writeShort(this.orientation[a]); 
        }
 
        outs.writeShort(this.velocity); 
 
    }
 
    public pose_t(byte[] data) throws IOException
    {
        this(new LCMDataInputStream(data));
    }
 
    public pose_t(DataInput ins) throws IOException
    {
        if (ins.readLong() != LCM_FINGERPRINT)
            throw new IOException("LCM Decode error: bad fingerprint");
 
        _decodeRecursive(ins);
    }
 
    public static poselcm.pose_t _decodeRecursiveFactory(DataInput ins) throws IOException
    {
        poselcm.pose_t o = new poselcm.pose_t();
        o._decodeRecursive(ins);
        return o;
    }
 
    public void _decodeRecursive(DataInput ins) throws IOException
    {
        this.robotid = ins.readByte();
 
        this.position = new short[(int) 3];
        for (int a = 0; a < 3; a++) {
            this.position[a] = ins.readShort();
        }
 
        this.orientation = new short[(int) 4];
        for (int a = 0; a < 4; a++) {
            this.orientation[a] = ins.readShort();
        }
 
        this.velocity = ins.readShort();
 
    }
 
    public poselcm.pose_t copy()
    {
        poselcm.pose_t outobj = new poselcm.pose_t();
        outobj.robotid = this.robotid;
 
        outobj.position = new short[(int) 3];
        System.arraycopy(this.position, 0, outobj.position, 0, 3); 
        outobj.orientation = new short[(int) 4];
        System.arraycopy(this.orientation, 0, outobj.orientation, 0, 4); 
        outobj.velocity = this.velocity;
 
        return outobj;
    }
 
}

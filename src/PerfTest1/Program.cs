using System;
using System.Collections.Generic;
using System.Linq;
using System.Security.Cryptography;
using System.Text;
using System.Threading.Tasks;
using System.Diagnostics;

namespace PerfTest1
{
    class Program
    {
        static void DoHash(ICryptoTransform hasher, bool log)
        {
            const int arraySize = 512;
            const int multiplier = 2*1024*1024;
            const double M = 1024 * 1024;
            const int bytes = arraySize * multiplier;
            const double mb = (double)bytes / M;

            var rnd = RandomNumberGenerator.Create();
            byte[] block = new byte[arraySize];
            rnd.GetNonZeroBytes(block);
            var timer = new Stopwatch();

            timer.Start();
            for (int i = 0; i < multiplier; ++i)
            {
                if (block[i % arraySize] != 0)
                {
                    hasher.TransformBlock(block, 0, block.Length, null, 0);
                }
                else
                {
                    // This if-else is just to confound the optimizer
                    System.Diagnostics.Debug.Assert(false);
                }
            }
            hasher.TransformFinalBlock(block, 0, block.Length);
            timer.Stop();

            if (log)
            {
                Console.WriteLine("{0}\n took {1} to hash {2} MB of data.", hasher.GetType(), timer.Elapsed, mb);
                Console.WriteLine("  That is {0} MB/sec.", mb / timer.Elapsed.TotalSeconds);
                Console.WriteLine();
            }
        }

        // Just dummy operations
        static void DoStuff(bool log)
        {
            const long arraySize = 1024;
            const long multiplier = 12 * 1024 * 1024;
            const double M = 1024 * 1024;
            const long bytes = arraySize * multiplier;
            const double mb = (double)bytes / M;

            var rnd = RandomNumberGenerator.Create();
            byte[] block = new byte[arraySize];
            rnd.GetNonZeroBytes(block);
            var timer = new Stopwatch();

            byte[] output = new byte[16];

            timer.Start();
            for (int i = 0; i < multiplier; ++i)
            {
                if (block[i % arraySize] == 42)
                {
                    output[i % 16] = (byte)'a';
                }
                else
                {
                    byte val = block[i % arraySize];
                    if (val != (byte)'a')
                    {
                        output[i % 16] = val;
                    }
                    else
                    {
                        output[i % 16] = 0;
                    }
                }
            }
            timer.Stop();

            if (log)
            {
                Console.WriteLine("{0}\n took {1} to TEST {2} MB of data.", "'42=answer'", timer.Elapsed, mb);
                Console.WriteLine("  That is {0} MB/sec.", mb / timer.Elapsed.TotalSeconds);
                Console.WriteLine();
            }
        }

        static void Main(string[] args)
        {
            Console.WriteLine("Warm-up ...");
            for (int i = 0; i < 2; ++i)
            {
                DoHash(MD5.Create(), false);
                DoStuff(false);
                DoHash(new MD5Cli.MD5(), false);
            }

            Console.WriteLine("Run Tests ...");
            DoHash(MD5.Create(), true);
            DoStuff(true);
            // DoHash(new MD5Cli.MD5(), true);
        }
    }
}

using System.IO;
using System.Linq;
using System.Collections.Generic;
using System;

class Program
{
    static void Main()
    {
        List<List<double>> lists = new();
        string filepath = "putty.txt";
        var lines = File.ReadAllLines(filepath);
        using (StreamWriter s = new StreamWriter("output.txt"))
        {
            foreach (var line in lines)
            {
                List<double> a = new List<double>();
                var values = line.Split(", ");
                foreach (var v in values)
                {
                    a.Add(Math.Abs(double.Parse(v)));
                }
                lists.Add(a);
            }
        }
        List<double> allNumbers = lists.SelectMany(list => list).ToList();
        List<(double center, double deviation)> deviations = CalculateDeviations(allNumbers, 0.2);

        foreach (var (center, deviation) in deviations)
        {
            Console.WriteLine($"Center: {center}, Deviation: ±{deviation}");
        }
    }
    static List<(double center, double deviation)> CalculateDeviations(List<double> numbers, double threshold)
    {
        List<List<double>> groupedNumbers = new List<List<double>>();

        foreach (var num in numbers)
        {
            bool added = false;

            foreach (var group in groupedNumbers)
            {
                if (group.Any(n => Math.Abs(n - num) <= 100))
                {
                    group.Add(num);
                    added = true;
                    break;
                }
            }

            if (!added)
            {
                groupedNumbers.Add(new List<double> { num });
            }
        }

        return groupedNumbers.Select(group =>
        {
            double center = group.Average();
            double deviation = group.Max() - center;
            return (center, deviation);
        }).ToList();
    }
}

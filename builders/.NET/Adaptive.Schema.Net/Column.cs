﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Adaptive.Schema.Net
{
    /// <summary>
    /// A column container inside of a columngroup
    /// </summary>
    public class Column : Container
    {
        public Column() { }

        // ISSUE Number?  I thought we had "Auto" and stuff..
        public int Weight { get; set; }
    }
}

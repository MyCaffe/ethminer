using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace GetWorkClient
{
    public class SimpleJsonFormatter
    {
        public SimpleJsonFormatter()
        {
        }

        public static string Format(SimpleJson.JsonObject json)
        {
            return format(json);
        }

        private static string format(SimpleJson.JsonObject json, string strTab = "", bool bAddComma = false)
        {
            string strOut = strTab + "{" + Environment.NewLine;
            string strOriginalTab = strTab;
            bool bOriginalAddComma = bAddComma;
            strTab += "  ";

            KeyValuePair<string, object>[] rgItems = json.ToArray();

            for (int i = 0; i < rgItems.Length; i++)
            {
                string strKey = rgItems[i].Key;
                SimpleJson.JsonArray children = rgItems[i].Value as SimpleJson.JsonArray;

                strOut += strTab + "\"" + strKey + "\":";

                if (children != null)
                {
                    if (children.Count > 1)
                        strOut += "[";

                    strOut += Environment.NewLine;

                    bAddComma = true;
                    int nIdx = 0;
                    foreach (object objChild in children)
                    {
                        if (nIdx >= children.Count - 1)
                            bAddComma = false;

                        SimpleJson.JsonObject child = objChild as SimpleJson.JsonObject;
                        if (child != null)
                        {
                            strOut += format(child, strTab + "  ", bAddComma);
                        }
                        else
                        {
                            strOut += strTab + "  ";

                            if (objChild == null)
                                strOut += "null";
                            else
                                strOut += "\"" + objChild.ToString() + "\"";

                            strOut += (bAddComma ? "," : "") + Environment.NewLine;
                        }
                        nIdx++;
                    }

                    if (children.Count > 1)
                        strOut += strTab + "]" + Environment.NewLine;
                }
                else
                {
                    SimpleJson.JsonObject child = rgItems[i].Value as SimpleJson.JsonObject;
                    if (child != null)
                    {
                        strOut += format(child, strTab + "  ", bAddComma);
                    }
                    else
                    {
                        if (rgItems[i].Value == null)
                            strOut += "null";
                        else
                            strOut += "\"" + rgItems[i].Value.ToString() + "\"";

                        if (bAddComma)
                            strOut += ",";

                        strOut += Environment.NewLine;
                    }
                }
            }

            strOut += strOriginalTab + "}" + (bOriginalAddComma ? "," : "") + Environment.NewLine;
            return strOut;
        }

        public static JsonData Extract(SimpleJson.JsonObject json)
        {
            JsonData root = new JsonData("root");
            extract(json, root);

            if (root.Children.Count == 1)
                return root.Children[0];

            return root;
        }

        private static void extract(SimpleJson.JsonObject json, JsonData root)
        {
            KeyValuePair<string, object>[] rgItems = json.ToArray();            

            for (int i = 0; i < rgItems.Length; i++)
            {
                string strKey = rgItems[i].Key;
                SimpleJson.JsonArray children = rgItems[i].Value as SimpleJson.JsonArray;

                JsonData data = new JsonData(strKey);

                if (children != null)
                {
                    int nIdx = 0;
                    foreach (object objChild in children)
                    {
                        JsonData child1 = new JsonData(nIdx.ToString());

                        SimpleJson.JsonObject child = objChild as SimpleJson.JsonObject;
                        if (child != null)
                        {
                            extract(child, child1);
                        }
                        else
                        {
                            if (objChild == null)
                                data.Values.Add("null");
                            else
                                data.Values.Add(objChild.ToString());
                        }
                        nIdx++;

                        data.Children.Add(child1);
                    }
                }
                else
                {
                    SimpleJson.JsonObject child = rgItems[i].Value as SimpleJson.JsonObject;

                    if (child != null)
                    {
                        extract(child, data);
                    }
                    else
                    {
                        if (rgItems[i].Value == null)
                            data.Values.Add("null");
                        else
                            data.Values.Add(rgItems[i].Value.ToString());
                    }
                }

                root.Children.Add(data);
            }
        }       
    }

    public class JsonData
    {
        string m_strName = null;
        List<string> m_rgstrValues = new List<string>();
        List<JsonData> m_rgChildren = new List<JsonData>();

        public JsonData(string strName)
        {
            m_strName = strName;
        }

        public string Name
        {
            get { return m_strName; }
        }

        public List<string> Values
        {
            get { return m_rgstrValues; }
        }

        public List<JsonData> Children
        {
            get { return m_rgChildren; }
        }

        public override string ToString()
        {
            return m_strName;
        }

        public long GetChildAsLong(string strName, long? lDefault = null)
        {
            foreach (JsonData child in Children)
            {
                if (child.Name == strName)
                {
                    if (child.Values.Count == 0)
                    {
                        if (!lDefault.HasValue)
                            throw new Exception("The child named '" + strName + "' has no values!");

                        return lDefault.GetValueOrDefault();
                    }

                    long lVal;
                    if (!long.TryParse(child.Values[0], out lVal))
                    {
                        if (!lDefault.HasValue)
                            throw new Exception("The child named '" + strName + "' has no valid long values!");

                        return lDefault.GetValueOrDefault();
                    }

                    return lVal;
                }
            }

            if (!lDefault.HasValue)
                throw new Exception("Could not find the child named '" + strName + "'!");

            return lDefault.GetValueOrDefault();
        }

        public double GetChildAsDouble(string strName, double? dfDefault = null)
        {
            foreach (JsonData child in Children)
            {
                if (child.Name == strName)
                {
                    if (child.Values.Count == 0)
                    {
                        if (!dfDefault.HasValue)
                            throw new Exception("The child named '" + strName + "' has no values!");

                        return dfDefault.GetValueOrDefault();
                    }

                    double dfVal;
                    if (!double.TryParse(child.Values[0], out dfVal))
                    {
                        if (!dfDefault.HasValue)
                            throw new Exception("The child named '" + strName + "' has no valid double values!");

                        return dfDefault.GetValueOrDefault();
                    }

                    return dfVal;
                }
            }

            if (!dfDefault.HasValue)
                throw new Exception("Could not find the child named '" + strName + "'!");

            return dfDefault.GetValueOrDefault();
        }

        public int GetChildAsInt(string strName, int? nDefault = null)
        {
            foreach (JsonData child in Children)
            {
                if (child.Name == strName)
                {
                    if (child.Values.Count == 0)
                    {
                        if (!nDefault.HasValue)
                            throw new Exception("The child named '" + strName + "' has no values!");

                        return nDefault.GetValueOrDefault();
                    }

                    int nVal;
                    if (!int.TryParse(child.Values[0], out nVal))
                    {
                        if (!nDefault.HasValue)
                            throw new Exception("The child named '" + strName + "' has no valid int values!");

                        return nDefault.GetValueOrDefault();
                    }

                    return nVal;
                }
            }

            if (!nDefault.HasValue)
                throw new Exception("Could not find the child named '" + strName + "'!");

            return nDefault.GetValueOrDefault();
        }

        public string GetChildAsString(string strName, string strDefault = null)
        {
            foreach (JsonData child in Children)
            {
                if (child.Name == strName)
                {
                    if (child.Values.Count == 0)
                    {
                        if (strDefault == null)
                            throw new Exception("The child named '" + strName + "' has no values!");

                        return strDefault;
                    }

                    return child.Values[0];
                }
            }

            if (strDefault == null)
                throw new Exception("Could not find the child named '" + strName + "'!");

            return strDefault;
        }
    }
}

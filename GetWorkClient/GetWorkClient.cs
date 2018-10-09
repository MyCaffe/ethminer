using RestSharp;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Net;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace GetWorkClient
{
    [Guid("C4BA9B15-521A-4C81-B256-A88EF56C35EF"), InterfaceType(ComInterfaceType.InterfaceIsIDispatch)]
    public interface IGetWorkClient
    {
        [DispId(1)]
        bool Connect(string strUrl);

        [DispId(2)]
        void Disconnect();

        [DispId(3)]
        string[] eth_getWork();

        [DispId(4)]
        string eth_submitWork(string strNonce, string strCurrentHeaderHash, string strMixDigest);

        [DispId(5)]
        string eth_submitHashrate(string strHash, string strID);

        [DispId(6)]
        string[] ParseHost(string strUri);
    }

    [Guid("8657A7C3-E544-401C-8275-7DA3F7BD65EC"), ClassInterface(ClassInterfaceType.None)]
    public class FarmClient : IGetWorkClient
    {
        string m_strUrl;

        public FarmClient()
        {
        }

        public bool Connect(string strUrl)
        {
            m_strUrl = strUrl;
            return true;
        }

        public void Disconnect()
        {          
        }

        public string[] ParseHost(string strUri)
        {
            List<string> rgstr = new List<string>();

            if (strUri.IndexOf("http://") < 0)
                strUri = "http://" + strUri;

            Uri uri = new Uri(strUri);

            rgstr.Add(uri.Host);
            rgstr.Add(uri.AbsolutePath);
            rgstr.Add(uri.Port.ToString());

            string strUi = uri.UserInfo;
            int nPos = strUri.IndexOf(',');

            string strUser = strUi;
            string strPw = "";

            if (nPos > 0)
            {
                strUser = strUri.Substring(0, nPos);
                strPw = strUri.Substring(nPos + 1);
            }

            rgstr.Add(strUser);
            rgstr.Add(strPw);

            return rgstr.ToArray();
        }

        public string[] eth_getWork()
        {
            string strCmd = "{\"jsonrpc\":\"2.0\",\"method\":\"eth_getWork\",\"params\":[],\"id\":1}";
            byte[] rgCmd = Encoding.UTF8.GetBytes(strCmd);

            HttpWebRequest request = (HttpWebRequest)WebRequest.Create(m_strUrl);
            request.ContentType = "application/json";
            request.Method = "POST";
            request.ContentLength = rgCmd.Length;
            Stream strm = request.GetRequestStream();
            strm.Write(rgCmd, 0, rgCmd.Length);
            strm.Close();

            HttpWebResponse response = (HttpWebResponse)request.GetResponse();
            string strStatus = response.StatusDescription;
            HttpStatusCode status = response.StatusCode;
            strm = response.GetResponseStream();
            string strContent = "";

            using (StreamReader sr = new StreamReader(strm))
            {
                strContent = sr.ReadToEnd();
            }

            SimpleJson.JsonObject json = SimpleJson.SimpleJson.DeserializeObject(strContent) as SimpleJson.JsonObject;
            List<string> rgstr = new List<string>();

            if (json.Count == 3)
            {
                if (!verifyValue(json, "jsonrpc", "2.0"))
                    return null;

                if (!verifyValue(json, "id", "1"))
                    return null;

                SimpleJson.JsonArray rgObj = getValue(json, "result") as SimpleJson.JsonArray;
                if (rgObj == null)
                    return null;

                if (rgObj.Count != 4)
                    return null;

                string strCurrentBlockHeader = rgObj[0].ToString();
                string strSeedHash = rgObj[1].ToString();
                string strBoundaryCondition = rgObj[2].ToString();

                rgstr.Add(strCurrentBlockHeader);
                rgstr.Add(strSeedHash);
                rgstr.Add(strBoundaryCondition);
            }

            if (rgstr.Count == 0)
                return null;

            return rgstr.ToArray();
        }

        public string eth_submitWork(string strNonce, string strCurrentHeaderHash, string strMixDigest)
        {
            string strCmd = "{\"jsonrpc\":\"2.0\",\"method\":\"eth_submitWork\",\"params\":[\"" + strNonce + "\",\"" + strCurrentHeaderHash + "\",\"" + strMixDigest + "\"],\"id\":73}";
            byte[] rgCmd = Encoding.UTF8.GetBytes(strCmd);

            HttpWebRequest request = (HttpWebRequest)WebRequest.Create(m_strUrl);
            request.ContentType = "application/json";
            request.Method = "POST";
            request.ContentLength = rgCmd.Length;
            Stream strm = request.GetRequestStream();
            strm.Write(rgCmd, 0, rgCmd.Length);
            strm.Close();

            HttpWebResponse response = (HttpWebResponse)request.GetResponse();
            string strStatus = response.StatusDescription;
            HttpStatusCode status = response.StatusCode;
            strm = response.GetResponseStream();
            string strContent = "";

            using (StreamReader sr = new StreamReader(strm))
            {
                strContent = sr.ReadToEnd();
            }

            SimpleJson.JsonObject json = SimpleJson.SimpleJson.DeserializeObject(strContent) as SimpleJson.JsonObject;
            if (json.Count == 3)
            {
                if (!verifyValue(json, "jsonrpc", "2.0"))
                    return json.ToString();

                if (!verifyValue(json, "id", "73"))
                    return json.ToString();

                bool? bResult = getValueAsBool(json, "result");
                if (bResult.HasValue)
                {
                    if (bResult.Value)
                        return "ACCEPTED";
                    else
                        return "REJECTED";
                }
            }

            return json.ToString();
        }

        public string eth_submitHashrate(string strHash, string strID)
        {
            string strCmd = "{\"jsonrpc\":\"2.0\",\"method\":\"eth_submitHashrate\",\"params\":[\"" + strHash + "\",\"" + strID + "\"],\"id\":73}";
            byte[] rgCmd = Encoding.UTF8.GetBytes(strCmd);

            HttpWebRequest request = (HttpWebRequest)WebRequest.Create(m_strUrl);
            request.ContentType = "application/json";
            request.Method = "POST";
            request.ContentLength = rgCmd.Length;
            Stream strm = request.GetRequestStream();
            strm.Write(rgCmd, 0, rgCmd.Length);
            strm.Close();

            HttpWebResponse response = (HttpWebResponse)request.GetResponse();
            string strStatus = response.StatusDescription;
            HttpStatusCode status = response.StatusCode;
            strm = response.GetResponseStream();
            string strContent = "";

            using (StreamReader sr = new StreamReader(strm))
            {
                strContent = sr.ReadToEnd();
            }

            SimpleJson.JsonObject json = SimpleJson.SimpleJson.DeserializeObject(strContent) as SimpleJson.JsonObject;
            if (json.Count == 3)
            {
                if (!verifyValue(json, "jsonrpc", "2.0"))
                    return json.ToString();

                if (!verifyValue(json, "id", "73"))
                    return json.ToString();

                bool? bResult = getValueAsBool(json, "result");
                if (bResult.HasValue)
                {
                    if (bResult.Value)
                        return "ACCEPTED";
                    else
                        return "REJECTED";
                }
            }

            return json.ToString();
        }

        private object getValue(SimpleJson.JsonObject json, string strKey)
        {
            string[] rgstrKeys = json.Keys.ToArray();
            object[] rgVal = json.Values.ToArray();

            for (int i = 0; i < rgstrKeys.Length; i++)
            {
                if (rgstrKeys[i] == strKey)
                    return rgVal[i];
            }

            return null;
        }

        public bool? getValueAsBool(SimpleJson.JsonObject json, string strKey)
        {
            object obj = getValue(json, strKey);

            if (obj.GetType() != typeof(bool))
                return null;

            return (bool)obj;
        }

        private bool verifyValue(SimpleJson.JsonObject json, string strKey, string strExpected)
        {
            string[] rgstrKeys = json.Keys.ToArray();
            object[] rgVal = json.Values.ToArray();

            for (int i = 0; i < rgstrKeys.Length; i++)
            {
                if (rgstrKeys[i] == strKey)
                {
                    string strVal = rgVal[i].ToString();
                    if (strVal == strExpected)
                        return true;
                    else
                        return false;
                }
            }

            return false;
        }
    }
}

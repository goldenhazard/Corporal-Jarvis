const secretKey = "get secret key in naver cloud platform"
const HmacSHA256 = require('crypto-js/hmac-sha256');
const EncBase64 = require('crypto-js/enc-base64');
module.exports = {
	chatbot: function(text,callback){
		var body =   JSON.stringify({
			"version": "v2",
			"userId": "U47b00b58c90f8e47428af8b7bddcda3d",
			"timestamp": new Date().getTime(),
			"bubbles": [
			  {
				"type": "text",
				"data" : {
				  "description" : text
				}
			  }
			],
			"event": "send"
		})
		var signatureHeader = HmacSHA256(body, secretKey).toString(EncBase64);
		var request = require('request');
		request.post({
			headers:  {"Content-Type": "application/json;UTF-8",
						"X-NCP-CHATBOT_SIGNATURE": signatureHeader
					  },
			url: 'https://finlxi2iu3.apigw.ntruss.com/speech/beta/',
			body: body
		},function(error,response,body){
			//console.log(error);
			var j_body = JSON.parse(body);
			callback(j_body.bubbles[0].data.description);
		});
	}
}
/*chatbot("클라우드봇",function(msg){
	console.log(msg);
});*/
varying vec2 texcoord;
varying vec3 position;

uniform sampler2D tex_0;

struct RiskObject{
	float wind[12];
	float value;
	float radius;
	vec2 center;
};

float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

void main()
{
	vec4 outColor = vec4(0.0, 0.0, 0.0, 1.0);
	vec4 tex_0_color = texture2D(tex_0, texcoord).zyxw;

	// make a fake circle
	int count_obj = 3;
	RiskObject obj[3];
	obj[0].center = vec2(-0.2, -0.2);
	obj[0].radius = 0.7;
	obj[0].value = 0.7;
	obj[1].center = vec2(0.2, 0.2);
	obj[1].radius = 0.6;
	obj[1].value = 0.4;
	obj[2].center = vec2(0.2, -0.3);
	obj[2].radius = 0.9;
	obj[2].value = 0.8;
	
	int i=0, j=0;
	obj[0].wind[0] = 0.05;
	obj[0].wind[1] = 0.03;
	obj[0].wind[2] = 0.12;
	obj[0].wind[3] = 0.07;
	obj[0].wind[4] = 0.03;
	obj[0].wind[5] = 0.15;
	obj[0].wind[6] = 0.05;
	obj[0].wind[7] = 0.10;
	obj[0].wind[8] = 0.08;
	obj[0].wind[9] = 0.06;
	obj[0].wind[10] = 0.12;
	obj[0].wind[11] = 0.14;	
	obj[1].wind[8] = 0.05;
	obj[1].wind[9] = 0.03;
	obj[1].wind[10] = 0.12;
	obj[1].wind[11] = 0.07;
	obj[1].wind[7] = 0.03;
	obj[1].wind[6] = 0.15;
	obj[1].wind[5] = 0.05;
	obj[1].wind[4] = 0.10;
	obj[1].wind[3] = 0.08;
	obj[1].wind[2] = 0.06;
	obj[1].wind[1] = 0.12;
	obj[1].wind[0] = 0.14;
	obj[2].wind[0] = 0.05;
	obj[2].wind[1] = 0.03;
	obj[2].wind[2] = 0.12;
	obj[2].wind[3] = 0.07;
	obj[2].wind[4] = 0.03;
	obj[2].wind[5] = 0.15;
	obj[2].wind[6] = 0.05;
	obj[2].wind[7] = 0.10;
	obj[2].wind[8] = 0.08;
	obj[2].wind[9] = 0.06;
	obj[2].wind[10] = 0.12;
	obj[2].wind[11] = 0.14;	
	/*
	for(i=0 ; i<count_obj ; i++){
		float scale = 0.0;
		for(j=0 ; j<12 ; j++){
			float r = rand(position.xy);
			obj[i].wind[j] = r;
			scale = scale + r;
		}
		scale = 1.0/scale;
		for(j=0 ; j<12 ; j++){
			obj[i].wind[j] = obj[i].wind[j] * scale;
		}
	}*/

	vec4 yellow = vec4(1.0, 1.0, 0.0, 1.0);
	vec4 red = vec4(1.0, 0.0, 0.0, 1.0);
	float col_val = 0;
	
	for(i=0 ; i<count_obj ; i++){
		float distance = sqrt((position.x-obj[i].center.x)*(position.x-obj[i].center.x) + (position.y-obj[i].center.y)*(position.y-obj[i].center.y));

		if(distance < obj[i].radius){
			float ratio_len = 1.0-(int)((distance/obj[i].radius) * 10)/10.0;
			float scale_final = 1.0;
			ratio_len *= ratio_len;
			//outColor += red * ratio_len + yellow * (1.0-ratio_len);

			vec2 p2c = normalize(obj[i].center - position.xy);
			vec2 t2c = normalize(vec2(1.0, 0.0));
			float deg = acos(dot(t2c, p2c)) * 57.2957795;
			for(j=0 ; j<12 ; j++){
				if(position.y > obj[i].center.y){
					if(deg > 30.0*j && deg < 30.0*(j+1)){
						// wind[0~5]
						float fan = (deg-30.0*j)/30.0;
						bool isClockwise = fan > 0.5;
						fan = isClockwise ? fan : 1-fan;
						fan = 1-fan+0.5;
						scale_final += obj[i].wind[j] * fan/* + (isClockwise ? obj[i].wind[j+1 > 11 ? 0 : j+1] : obj[i].wind[j-1 < 0 ? 11 : j-1])*/;
						break;
					}
				}
				else{
					if(180.0-deg > 30.0*j && 180.0-deg < 30.0*(j+1)){
						// wind[6~11]
						float fan = (180.0-deg-30.0*j)/30.0;
						bool isClockwise = fan > 0.5;
						fan = isClockwise ? fan : 1-fan;
						fan = 1-fan+0.5;
						scale_final += obj[i].wind[j] * fan/* + (isClockwise ? obj[i].wind[j+1 > 11 ? 0 : j+1] : obj[i].wind[j-1 < 0 ? 11 : j-1])*/;
						break;
					}
				}
			}
			col_val += ratio_len * obj[i].value;
			outColor += vec4(0.1, 0.0, 0.0, 0.0);
		}
	}
	//outColor /= count_obj;
	//col_val /= count_obj;
	//if(col_val != 0.0){
	//	outColor = red * col_val + yellow * (1.0-col_val);
	//	outColor.a = 1.0;
	//}
	

	gl_FragColor = outColor;
	//gl_FragColor = vec4(1.0, 1.0, 0.0, 1.0);
}
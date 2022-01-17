#version 430
uniform float antialias;
uniform float linewidth;
uniform float miter_limit;

in float v_length;
in float v_alpha;
in vec2 v_texcoord;
in vec2 v_bevel_distance;

in vec4 fColor;
out vec4 outColor;

void main()
{
    float distance = v_texcoord.y;

    // Round join (instead of miter)
     if (v_texcoord.x < 0.0)          { distance = length(v_texcoord); }
     else if(v_texcoord.x > v_length) { distance = length(v_texcoord - vec2(v_length, 0.0)); }

    float d = abs(distance) - linewidth/2.0 + antialias;

    // Miter limit
//    float m = miter_limit*(linewidth/2.0);
//    if (v_texcoord.x < 0.0)          { d = max(v_bevel_distance.x-m ,d); }
//    else if(v_texcoord.x > v_length) { d = max(v_bevel_distance.y-m ,d); }

    float alpha = 1.0;
    if( d > 0.0 )
    {
        alpha = d/(antialias);
        alpha = exp(-alpha*alpha);
    }
    outColor = vec4(fColor.r, fColor.g, fColor.b, fColor.a*alpha*v_alpha);
    if (outColor.a == 0.) gl_FragDepth = 1.0;
    else gl_FragDepth = gl_FragCoord.z;
//    outColor = vec4(1, 1, 1, alpha*v_alpha);
//    gl_FragColor = vec4(1, 0, 0, 1);
}

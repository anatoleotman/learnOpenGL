#version 430

layout(lines_adjacency) in;
layout(triangle_strip, max_vertices = 4) out;

in vec4 vColor[]; // Sortie du vertex shader, pour chaque sommet

uniform mat4 projection;
uniform vec2 viewport;

uniform float antialias;
uniform float linewidth;
uniform float miter_limit;

out float v_length;
out float v_alpha;
out vec2 v_texcoord;
out vec2 v_bevel_distance;
out vec4 fColor;
//__________________________________________________________________________________________________
float compute_u(vec2 p0, vec2 p1, vec2 p)
{
    // Projection p' of p such that p' = p0 + u*(p1-p0)
    // Then  u *= lenght(p1-p0)
    vec2 v = p1 - p0;
    float l = length(v);
    return ((p.x-p0.x)*v.x + (p.y-p0.y)*v.y) / l;
}
//__________________________________________________________________________________________________
float line_distance(vec2 p0, vec2 p1, vec2 p)
{
    // Projection p' of p such that p' = p0 + u*(p1-p0)
    vec2 v = p1 - p0;
    float l2 = v.x*v.x + v.y*v.y;
    float u = ((p.x-p0.x)*v.x + (p.y-p0.y)*v.y) / l2;

    // h is the prpjection of p on (p0,p1)
    vec2 h = p0 + u*v;

    return length(p-h);
}
//__________________________________________________________________________________________________
vec2 NDC_to_viewport(vec4 position, vec2 viewport)
{
    vec2 p = position.xy/position.w;
    return (p+1.0)/2.0 * viewport;
}
//__________________________________________________________________________________________________
vec4 viewport_to_NDC(vec2 position, vec2 viewport)
{
    return vec4(2.0*(position/viewport) - 1.0, 0.0, 1.0);
}
//__________________________________________________________________________________________________
vec4 viewport_to_NDC(vec3 position, vec2 viewport)
{
    return vec4(2.0*(position.xy/viewport) - 1.0, position.z, 1.0);
}
//__________________________________________________________________________________________________
void main(void)
{
    // Get the four vertices passed to the shader
    vec3 pp0 = gl_in[0].gl_Position.xyz; // start of previous segment
    vec3 pp1 = gl_in[1].gl_Position.xyz; // end of previous segment, start of current segment
    vec3 pp2 = gl_in[2].gl_Position.xyz; // end of current segment, start of next segment
    vec3 pp3 = gl_in[3].gl_Position.xyz; // end of next segment

    // transform prev/curr/next
    vec4 p0_ = projection * vec4(pp0, 1);
    vec4 p1_ = projection * vec4(pp1, 1);
    vec4 p2_ = projection * vec4(pp2, 1);
    vec4 p3_ = projection * vec4(pp3, 1);

    // prev/curr/next in viewport coordinates
    vec2 p0 = NDC_to_viewport(p0_, viewport.xy);
    vec2 p1 = NDC_to_viewport(p1_, viewport.xy);
    vec2 p2 = NDC_to_viewport(p2_, viewport.xy);
    vec2 p3 = NDC_to_viewport(p3_, viewport.xy);
    // Determine the direction of each of the 3 segments (previous, current, next)
    vec2 v0 = normalize(p1 - p0);
    vec2 v1 = normalize(p2 - p1);
    vec2 v2 = normalize(p3 - p2);

    // Determine the normal of each of the 3 segments (previous, current, next)
    vec2 n0 = vec2(-v0.y, v0.x);
    vec2 n1 = vec2(-v1.y, v1.x);
    vec2 n2 = vec2(-v2.y, v2.x);

    // Determine miter lines by averaging the normals of the 2 segments
    vec2 miter_a = normalize(n0 + n1); // miter at start of current segment
    vec2 miter_b = normalize(n1 + n2); // miter at end of current segment

    // Determine the length of the miter by projecting it onto normal
    vec2 p, v;
    float z;
    float d;
    float w = linewidth/2.0 + 1.5*antialias;
    v_length = length(p2-p1);

    float length_a = w / dot(miter_a, n1);
    float length_b = w / dot(miter_b, n1);

//    float m = miter_limit*linewidth/2.0;

    // Angle between prev and current segment (sign only)
    float d0 = +1.0;
    if( (v0.x*v1.y - v0.y*v1.x) > 0 ) { d0 = -1.0;}

    // Angle between current and next segment (sign only)
    float d1 = +1.0;
    if( (v1.x*v2.y - v1.y*v2.x) > 0 ) { d1 = -1.0; }

    // Generate the triangle strip
    z = p1_.z / p1_.w;

    v_alpha = 1.0;
    // Cap at start
    if ( p0 == p1 )
    {
        p = p1 - w*v1 + w*n1;
        v_texcoord = vec2(-w, +w);
        if (p2 == p3) v_alpha = 0.0; // to separate line strip
        fColor = vColor[0];
    // Regular join
    }
    else
    {
        p = p1 + length_a * miter_a;
        v_texcoord = vec2(compute_u(p1,p2,p), +w);
        fColor = vColor[1];
    }
    gl_Position = viewport_to_NDC(vec3(p, z), viewport);
    v_bevel_distance.x = +d0*line_distance(p1+d0*n0*w, p1+d0*n1*w, p);
    v_bevel_distance.y =    -line_distance(p2+d1*n1*w, p2+d1*n2*w, p);
    EmitVertex();

    v_alpha = 1.0;
    // Cap at start
    if ( p0 == p1 )
    {
        p = p1 - w*v1 - w*n1;
        v_texcoord = vec2(-w, -w);
        if (p2 == p3) v_alpha = 0.0; // to separate line strip
        fColor = vColor[0];
    // Regular join
    }
    else
    {
        p = p1 - length_a * miter_a;
        v_texcoord = vec2(compute_u(p1,p2,p), -w);
        fColor = vColor[1];
    }
    gl_Position = viewport_to_NDC(vec3(p, z), viewport);
    v_bevel_distance.x = -d0*line_distance(p1+d0*n0*w, p1+d0*n1*w, p);
    v_bevel_distance.y =    -line_distance(p2+d1*n1*w, p2+d1*n2*w, p);
    EmitVertex();

    z = p2_.z / p2_.w;
    v_alpha = 1.0;
    // Cap at end
    if ( p2 == p3 )
    {
        p = p2 + w*v1 + w*n1;
        v_texcoord = vec2(v_length+w, +w);
        if (p0 == p1) v_alpha = 0.0; // to separate line strip
        fColor = vColor[3];
    // Regular join
    }
    else
    {
        p = p2 + length_b * miter_b;
        v_texcoord = vec2(compute_u(p1,p2,p), +w);
        fColor = vColor[2];
    }
    gl_Position = viewport_to_NDC(vec3(p, z), viewport);
    v_bevel_distance.x =    -line_distance(p1+d0*n0*w, p1+d0*n1*w, p);
    v_bevel_distance.y = +d1*line_distance(p2+d1*n1*w, p2+d1*n2*w, p);
    EmitVertex();

    v_alpha = 1.0;
    // Cap at end
    if ( p2 == p3 )
    {
        p = p2 + w*v1 - w*n1;
        v_texcoord = vec2(v_length+w, -w);
        if (p0 == p1) v_alpha = 0.0; // to separate line strip
        fColor = vColor[3];
    // Regular join
    }
    else
    {
        p = p2 - length_b * miter_b;
        v_texcoord = vec2(compute_u(p1,p2,p), -w);
        fColor = vColor[2];
    }
    gl_Position = viewport_to_NDC(vec3(p, z), viewport);
    v_bevel_distance.x =    -line_distance(p1+d0*n0*w, p1+d0*n1*w, p);
    v_bevel_distance.y = -d1*line_distance(p2+d1*n1*w, p2+d1*n2*w, p);
    EmitVertex();

    EndPrimitive();
}
